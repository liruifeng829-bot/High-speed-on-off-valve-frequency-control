function [Tmetric, curvesWide] = new_run_compare_sin()
% Compare fixed/old/base on sine model with richer plots + metrics + Excel export
% Model: VVA_pressure_dynamic_basefreq_sin
% StopTime: 6 s
%
% Required logsout signals (names in Simulink logging):
%   P, Pr, fs, U1, U2
% Duty is optional (will auto-detect from candidates)

%% ===== config =====
mdl    = 'VVA_pressure_dynamic_basefreq_sin';
stopT  = '6';                     % seconds (string is OK for sim)
modes  = [1 2 3];                 % 1=fixed, 2=old, 3=base
labels = {'fixed','old','base'};

% thresholds / settings
uThr         = 0.5;               % digital toggle threshold
settleBand   = 0.01;              % (not used for sine, but kept)
tSS_start    = 0.5;               % steady-state window start (s), tune if needed
Ts_assume    = 1e-4;              % fallback if no time vector

% duty candidates: put your real logged duty name first if known
dutyNames = {'d_feas','d','D','d_raw','d_cmd','duty','PWM_duty'};

% output file
outXlsx = sprintf('compare_%s_%ss.xlsx', mdl, stopT);

%% ===== run =====
res = struct([]);
for i = 1:numel(modes)
    resetPersistents();  % important for fair comparison across runs
    assignin('base','freq_mode',modes(i));

    simOut = sim(mdl, 'StopTime', stopT);
    logs   = simOut.logsout;

    % print what is actually in logsout once
    if i == 1
        names = getElementNamesSafe(logs);
        fprintf('logsout signals (%d): %s\n', numel(names), strjoin(names, ', '));
    end

    % required
    [tP,  P ] = getTS_noWarn(logs,'P');
    [tR,  Pr] = getTS_noWarn(logs,'Pr');
    [tfs, fs] = getTS_noWarn(logs,'fs');
    [t1,  U1] = getTS_noWarn(logs,'U1');
    [t2,  U2] = getTS_noWarn(logs,'U2');

    % choose common time base
    if ~isempty(tP)
        t = tP(:);
    elseif ~isempty(tR)
        t = tR(:);
    elseif isfield(simOut,'tout') && ~isempty(simOut.tout)
        t = simOut.tout(:);
    else
        t = (0:Ts_assume:str2double(stopT)).';
    end

    % interpolate: analog linear, digital previous
    P  = interpTo(tP,  P,  t, 'linear');
    Pr = interpTo(tR,  Pr, t, 'linear');
    fs = interpTo(tfs, fs, t, 'linear');
    U1 = interpTo(t1,  U1, t, 'previous');
    U2 = interpTo(t2,  U2, t, 'previous');

    % optional duty
    d = []; d_name = '';
    for k = 1:numel(dutyNames)
        [td, dv] = getTS_noWarn(logs, dutyNames{k});
        if ~isempty(dv)
            d      = interpTo(td, dv, t, 'linear');
            d_name = dutyNames{k};
            break;
        end
    end

    % error
    e = Pr - P;

    % metrics (overall)
    RMSE    = sqrt(mean(e.^2));
    MAE     = mean(abs(e));
    maxAbsE = max(abs(e));
    IAE     = trapz(t, abs(e));
    ISE     = trapz(t, e.^2);

    % steady-state metrics (exclude initial transient)
    t_ss0 = 1.0;
    idx_ss  =(t >= t_ss0);
    if any(idx_ss)
        eSS     = e(idx_ss);
        RMSE_ss = sqrt(mean(e(idx_ss).^2));
        MAE_ss  = mean(abs(e(idx_ss)));
    else
        RMSE_ss = NaN; MAE_ss = NaN;
    end

    % switching counts
    N1  = countToggles(U1, uThr);
    N2  = countToggles(U2, uThr);
    Nsw = N1 + N2;

    Tsim    = t(end) - t(1);
    sw_rate = Nsw / max(Tsim, eps);

    fmean = mean(fs);
    fmin  = min(fs);
    fmax  = max(fs);

    % cumulative toggles curve (for plotting/export)
    cumN1  = cumulativeToggles(U1, uThr);
    cumN2  = cumulativeToggles(U2, uThr);
    cumN   = cumN1 + cumN2;

    % store
    res(i).lbl = labels{i};
    res(i).mode = modes(i);
    res(i).t = t;
    res(i).Pr = Pr; res(i).P = P; res(i).e = e;
    res(i).fs = fs;
    res(i).U1 = U1; res(i).U2 = U2;
    res(i).d = d; res(i).d_name = d_name;
    res(i).cumN = cumN;

    res(i).RMSE = RMSE; res(i).MAE = MAE; res(i).maxAbsE = maxAbsE;
    res(i).IAE = IAE; res(i).ISE = ISE;
    res(i).RMSE_ss = RMSE_ss; res(i).MAE_ss = MAE_ss;
    res(i).N1 = N1; res(i).N2 = N2; res(i).Nsw = Nsw; res(i).sw_rate = sw_rate;
    res(i).fmean = fmean; res(i).fmin = fmin; res(i).fmax = fmax;
end

%% ===== metrics table =====
Tmetric = table( ...
    string({res.lbl})', ...
    [res.RMSE]', [res.MAE]', [res.maxAbsE]', [res.IAE]', [res.ISE]', ...
    [res.RMSE_ss]', [res.MAE_ss]', ...
    [res.N1]', [res.N2]', [res.Nsw]', [res.sw_rate]', ...
    [res.fmean]', [res.fmin]', [res.fmax]', ...
    'VariableNames', {'mode','RMSE','MAE','maxAbsE','IAE','ISE','RMSE_ss','MAE_ss', ...
                      'N1','N2','Nsw','sw_rate','fmean','fmin','fmax'} );
disp(Tmetric);

% quick check vs fixed
fixedN = res(1).Nsw;
baseN  = res(3).Nsw;
fprintf('Base vs Fixed Nsw reduction = %.2f%% (fixed=%g, base=%g)\n', ...
    100*(fixedN-baseN)/max(fixedN,eps), fixedN, baseN);

%% ===== plots =====
figure('Name','Compare: Pressure/Frequency/Error/Duty/U1/U2');
tl = tiledlayout(3,2,'TileSpacing','compact','Padding','compact');

% Pressure
nexttile; hold on; grid on;
plot(res(1).t, res(1).Pr, 'k--', 'DisplayName','Reference');
for i=1:numel(res)
    plot(res(i).t, res(i).P, 'DisplayName',[res(i).lbl ' P']);
end
xlabel('Time (s)'); ylabel('Pressure (Pa)'); title('Pressure Tracking');
legend('show','Location','best');

% Error
nexttile; hold on; grid on;
for i=1:numel(res)
    plot(res(i).t, res(i).e, 'DisplayName',[res(i).lbl ' e=Pr-P']);
end
xlabel('Time (s)'); ylabel('Error (Pa)'); title('Tracking Error');
legend('show','Location','best');

% Frequency
nexttile; hold on; grid on;
for i=1:numel(res)
    plot(res(i).t, res(i).fs, 'DisplayName',[res(i).lbl ' fs']);
end
xlabel('Time (s)'); ylabel('Frequency (Hz)'); title('PWM Base Frequency');
legend('show','Location','best');

% Duty
nexttile; hold on; grid on;
hasDuty = false;
for i=1:numel(res)
    if ~isempty(res(i).d)
        hasDuty = true;
        dn = res(i).d_name; if isempty(dn), dn='d'; end
        plot(res(i).t, res(i).d, 'DisplayName',[res(i).lbl ' ' dn]);
    end
end
xlabel('Time (s)'); ylabel('Duty'); title('Duty Signal (logged)');
legend('show','Location','best');
if ~hasDuty
    text(0.05,0.6,'Duty not found in logsout.', 'Units','normalized');
    text(0.05,0.45,'Enable logging for d\_feas (or add its name to dutyNames).', 'Units','normalized');
end

% U1
nexttile; hold on; grid on;
for i=1:numel(res)
    stairs(res(i).t, res(i).U1, 'DisplayName',[res(i).lbl ' U1']);
end
xlabel('Time (s)'); ylabel('U1'); title('Valve Drive U1 (digital)');
ylim([-0.1 1.1]); yticks([0 1]);
legend('show','Location','best');

% U2
nexttile; hold on; grid on;
plot([res(1).t(1) res(1).t(end)], [0 0], 'k:', 'DisplayName','U2=0 baseline');
for i=1:numel(res)
    stairs(res(i).t, res(i).U2, 'DisplayName',[res(i).lbl ' U2']);
end
xlabel('Time (s)'); ylabel('U2'); title('Valve Drive U2 (digital)');
ylim([-0.1 1.1]); yticks([0 1]);
legend('show','Location','best');

% cumulative switching
figure('Name','Compare: Cumulative Switching Count'); hold on; grid on;
for i=1:numel(res)
    plot(res(i).t, res(i).cumN, 'DisplayName',[res(i).lbl ' cumN']);
end
xlabel('Time (s)'); ylabel('Cumulative toggles'); title('Cumulative Switching Count');
legend('show','Location','best');

%% ===== export to Excel (Origin-friendly wide table) =====
% Build wide table on a common time base (use fixed run time as reference)
t = res(1).t(:);
curvesWide = table(t, res(1).Pr(:), 'VariableNames', {'t','Pr'});

for i=1:numel(res)
    tag = res(i).lbl;

    curvesWide.(sprintf('P_%s',tag))   = res(i).P(:);
    curvesWide.(sprintf('e_%s',tag))   = res(i).e(:);
    curvesWide.(sprintf('fs_%s',tag))  = res(i).fs(:);
    curvesWide.(sprintf('U1_%s',tag))  = res(i).U1(:);
    curvesWide.(sprintf('U2_%s',tag))  = res(i).U2(:);
    curvesWide.(sprintf('cumN_%s',tag))= res(i).cumN(:);

    if ~isempty(res(i).d)
        curvesWide.(sprintf('d_%s',tag)) = res(i).d(:);
    end
end

% overwrite file
if exist(outXlsx,'file'), delete(outXlsx); end
writetable(Tmetric, outXlsx, 'Sheet','metrics');
writetable(curvesWide, outXlsx, 'Sheet','curves_wide');

fprintf('Exported curves + metrics to: %s\n', outXlsx);
fprintf('Saved in folder: %s\n', pwd);

end

%% ===== helpers =====
function resetPersistents()
% Clear MATLAB Function blocks / m-files that use persistent variables
% Add names here if you have more persistent-state functions
clear VarFreqPWM BaseFreqOpt FreqPolicy frequency Duty_cycle_compensation DutyComp_D
end

function names = getElementNamesSafe(logs)
try
    names = logs.getElementNames;
    if isstring(names), names = cellstr(names); end
catch
    names = {};
end
end

function [t, x] = getTS_noWarn(logs, name)
% No warnings: check name exists before getElement
t = []; x = [];
names = getElementNamesSafe(logs);
if ~any(strcmp(names, name)), return; end
el = logs.getElement(name);
ts = el.Values;                  % timeseries
t  = ts.Time(:);
x  = ts.Data(:);
end

function y = interpTo(t0, x0, t, method)
y = [];
if isempty(x0) || isempty(t0) || isempty(t), return; end
[t0u, ia] = unique(t0(:));
x0u = x0(:); x0u = x0u(ia);
y = interp1(t0u, x0u, t(:), method, 'extrap');
end

function N = countToggles(u, thr)
if isempty(u), N = NaN; return; end
b = (u(:) > thr);
N = sum(diff(b) ~= 0);
end

function cumN = cumulativeToggles(u, thr)
if isempty(u), cumN = []; return; end
b = (u(:) > thr);
cumN = cumsum(diff(b) ~= 0);
cumN = [0; cumN(:)];   % align length to time vector
end
function export_for_origin(res, T, filename)
% res(i).lbl: 'fixed'/'old'/'base'
% res(i).t, Pr, P, e, fs, d, U1, U2

writetable(T, filename, 'Sheet', 'metrics');

t = res(1).t;

writeSig = @(sheet, yname, gety) writetable( ...
    table(t, gety(1), gety(2), gety(3), ...
    'VariableNames', {'t', ['fixed_' yname], ['old_' yname], ['base_' yname]}), ...
    filename, 'Sheet', sheet);

writeSig('Pressure', 'P',  @(k) res(k).P);
writetable(table(t, res(1).Pr, 'VariableNames', {'t','Pr'}), filename, 'Sheet','Reference');

writeSig('Error',    'e',  @(k) res(k).e);
writeSig('Freq',     'fs', @(k) res(k).fs);

% duty 可能某些模式为空，做一下保护
d1 = fillEmpty(res(1).d, t); d2 = fillEmpty(res(2).d, t); d3 = fillEmpty(res(3).d, t);
writetable(table(t, d1, d2, d3, 'VariableNames', {'t','fixed_d','old_d','base_d'}), filename, 'Sheet','Duty');

writeSig('U1', 'U1', @(k) res(k).U1);
writeSig('U2', 'U2', @(k) res(k).U2);

disp(['Exported for Origin: ' filename]);

end

function y = fillEmpty(yin, t)
if isempty(yin), y = nan(size(t)); else, y = yin; end
end
