function F = objfun_ga(x)
%OBJFUN_GA  Multi-objective objective for gamultiobj (NSGA-II)
%
% Decision variables:
%   x = [kf, f_min, lambda_lp]
%
% Objectives (minimize):
%   J1 = normalized RMSE of pressure tracking (dimensionless)
%   J2 = switching count Nsw (integer-like)
%
% Notes:
% - This function is self-contained (no external getTimeData.m needed).
% - It auto-fixes unit mismatch between Pr (~1e5 Pa) and P (~1~10 bar/MPa-like).
%
% Model:
%   mdl = 'VVA_pressure_dynamic_basefreq_sin_GA';

    %% ---------------- user config ----------------
    mdl          = 'VVA_pressure_dynamic_basefreq_sin_GA';  % 你的 GA 模型名
    freq_mode_val = 3;                                      % 强制走动态优化算法分支
    PENALTY      = 1e12;                                    % 仿真失败惩罚
    NORMALIZE_MODE = "rms";                                 % "rms" or "range"
    DEBUG = false;                                          % 手动调试可置 true

    %% ---------------- decode & guard ----------------
    x = double(x(:));
    if numel(x) ~= 3 || any(~isfinite(x))
        F = [PENALTY, PENALTY];
        return;
    end

    kf_val   = x(1);
    fmin_val = x(2);
    lam_val  = x(3);

    % 轻度钳位（GA 本身有边界，这里主要防 NaN/Inf 或极端值导致模型报错）
    kf_val   = max(0.0, min(1.0, kf_val));
    lam_val  = max(0.0, min(1.0, lam_val));
    fmin_val = max(1.0, fmin_val);

    %% ---------------- ensure model loaded ----------------
    persistent MODEL_LOADED
    try
        if isempty(MODEL_LOADED) || ~bdIsLoaded(mdl)
            load_system(mdl);
            MODEL_LOADED = true;
        end
    catch
        F = [PENALTY, PENALTY];
        return;
    end

    %% ---------------- run simulation ----------------
    try
        in = Simulink.SimulationInput(mdl);

        % 强制走你的动态频率优化分支
        in = in.setVariable('freq_mode', freq_mode_val);

        % 注入 GA 变量（按你模型里常见命名 + 兼容别名）
        in = in.setVariable('kf',        kf_val);
        in = in.setVariable('f_min',     fmin_val);
        in = in.setVariable('lambda_lp', lam_val);

        % 兼容别名（如果模型某处用这些名字）
        in = in.setVariable('fmin',      fmin_val);
        in = in.setVariable('lambda_f',  lam_val);
        in = in.setModelParameter('StopTime','1.5');
        % 建议先 normal 保证稳定（稳定后再考虑 rapid accelerator）
        in = in.setModelParameter('SimulationMode','accelerator');

        simOut = sim(in);

        %% -------- extract signals (robust name matching) --------
        % 参考压力 Pr(t)
        [tPr, Pr] = getSigByCandidates(simOut, {'Pr','P_ref','Pref','Pr_ref','Pr_out','out.Pr'});

        % 实际压力 P(t)
        [tP,  P ] = getSigByCandidates(simOut, {'P','P_out','Pact','P_actual','out.P'});

        % 阀驱动 U1/U2
        [tU1, U1] = getSigByCandidates(simOut, {'U1','u1','u_1'});
        [tU2, U2] = getSigByCandidates(simOut, {'U2','u2','u_2'});

        if isempty(Pr) || isempty(P) || isempty(U1) || isempty(U2)
            F = [PENALTY, PENALTY];
            return;
        end

        %% -------- clean signals --------
        [tPr, Pr] = cleanTimeSeries(tPr, Pr);
        [tP,  P ] = cleanTimeSeries(tP,  P );
        [~,   U1] = cleanTimeSeries(tU1, U1);
        [~,   U2] = cleanTimeSeries(tU2, U2);

        if numel(tPr) < 5 || numel(tP) < 5
            F = [PENALTY, PENALTY];
            return;
        end

        %% -------- align time base: interpolate P onto Pr time --------
        P_on_Pr = interp1(tP, P, tPr, 'linear', 'extrap');

        %% -------- auto unit fix (critical for your “RMSE≈Pr_rms => J1≈1” issue) --------
        % 如果 Pr ~ 1e5 而 P ~ 1~10，则 P 很可能是 bar/MPa 量纲，需要乘 1e5/1e6
        Pr_max = max(abs(Pr));
        P_max  = max(abs(P_on_Pr));

        if Pr_max > 0 && P_max > 0
            ratio_Pr_over_P = Pr_max / (P_max + eps);
            ratio_P_over_Pr = P_max  / (Pr_max + eps);

            % 只在量级差非常明显时才修正（避免误判）
            if ratio_Pr_over_P > 1e3
                gain = 10^round(log10(ratio_Pr_over_P));
                % 合理增益范围（通常 1e5 或 1e6）
                if gain >= 1e3 && gain <= 1e8
                    P_on_Pr = P_on_Pr * gain;
                    if DEBUG
                        fprintf('[unit-fix] P scaled by %g to match Pr.\n', gain);
                    end
                end
            elseif ratio_P_over_Pr > 1e3
                % 极少见：若反过来 Pr 很小、P 很大（一般不建议改 Pr，但这里留个兜底）
                gain = 10^round(log10(ratio_P_over_Pr));
                if gain >= 1e3 && gain <= 1e8
                    Pr = Pr * gain;
                    if DEBUG
                        fprintf('[unit-fix] Pr scaled by %g to match P.\n', gain);
                    end
                end
            end
        end

        %% -------- objective 1: normalized RMSE --------
        e = Pr - P_on_Pr;
        rmse = sqrt(mean(e.^2));

        switch NORMALIZE_MODE
            case "rms"
                denom = sqrt(mean(Pr.^2)) + eps;
            case "range"
                denom = (max(Pr) - min(Pr)) + eps;
            otherwise
                denom = sqrt(mean(Pr.^2)) + eps;
        end
        J1 = rmse / denom;   % dimensionless

        %% -------- objective 2: switching count Nsw --------
        U1b = double(U1 > 0.5);
        U2b = double(U2 > 0.5);
        J2  = sum(abs(diff(U1b))) + sum(abs(diff(U2b)));

        %% -------- safety checks --------
        if ~isfinite(J1) || ~isfinite(J2) || J1 < 0 || J2 < 0
            F = [PENALTY, PENALTY];
            return;
        end

        % 可选：若误差爆炸也惩罚
        if rmse > 1e9 || any(abs(Pr) > 1e10) || any(abs(P_on_Pr) > 1e10)
            F = [PENALTY, PENALTY];
            return;
        end

        if DEBUG
            fprintf('Pr_max=%g, P_max=%g, rmse=%g, J1=%g, J2=%g\n', ...
                max(abs(Pr)), max(abs(P_on_Pr)), rmse, J1, J2);
        end

        F = double([J1, J2]);

    catch ME
        if DEBUG
            disp(getReport(ME,'extended'));
        end
        F = [PENALTY, PENALTY];
    end
end

%% ===================== local helper functions =====================

function [t,y] = getSigByCandidates(simOut, candidates)
% Try simOut.get(name) for each candidate; parse into (t,y).
    t = []; y = [];
    names = {};
    try
        names = simOut.who;  % sometimes returns cellstr
    catch
    end

    for i = 1:numel(candidates)
        name = candidates{i};
        try
            if ~isempty(names) && iscell(names)
                if ~ismember(name, names)
                    % still try simOut.get (some outputs not shown in who)
                end
            end
            obj = simOut.get(name);
            [t,y] = extractTimeData(obj);
            if ~isempty(t) && ~isempty(y)
                return;
            end
        catch
            % continue
        end
    end
end

function [t,y] = extractTimeData(x)
% Convert different Simulink logging formats into (t,y) numeric vectors.
    t = []; y = [];

    % timeseries
    if isa(x,'timeseries')
        t = double(x.Time(:));
        y = double(x.Data(:));
        return;
    end

    % struct format from To Workspace (Structure with time)
    if isstruct(x) && isfield(x,'time') && isfield(x,'signals')
        try
            t = double(x.time(:));
            v = x.signals(1).values;
            y = double(v(:));
            return;
        catch
        end
    end

    % Dataset
    if isa(x,'Simulink.SimulationData.Dataset')
        try
            if x.numElements >= 1
                el = x.getElement(1);
                if isprop(el,'Values')
                    v = el.Values;
                    if isa(v,'timeseries')
                        t = double(v.Time(:));
                        y = double(v.Data(:));
                        return;
                    end
                end
            end
        catch
        end
    end

    % Simulink.SimulationData.Signal (sometimes)
    if isa(x,'Simulink.SimulationData.Signal')
        try
            v = x.Values;
            if isa(v,'timeseries')
                t = double(v.Time(:));
                y = double(v.Data(:));
                return;
            end
        catch
        end
    end

    % numeric
    if isnumeric(x) || islogical(x)
        y = double(x(:));
        t = double((0:numel(y)-1).');
        return;
    end
end

function [t,y] = cleanTimeSeries(t,y)
% Make t increasing & unique; make y column double; drop NaN/Inf.
    t = double(t(:));
    y = double(y(:));

    mask = isfinite(t) & isfinite(y);
    t = t(mask);
    y = y(mask);

    if isempty(t)
        return;
    end

    % unique time (stable)
    [t, ia] = unique(t, 'stable');
    y = y(ia);

    % if not strictly increasing, sort
    if any(diff(t) <= 0)
        [t, idx] = sort(t);
        y = y(idx);
        [t, ia] = unique(t, 'stable');
        y = y(ia);
    end
end
