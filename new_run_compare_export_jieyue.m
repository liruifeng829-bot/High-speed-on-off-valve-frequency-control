function [T, res, xlsxFile] = new_run_compare_export()
% Compare fixed/old/base and export time-series + metrics to Excel for Origin
%
% Output Excel structure:
%   Sheet "summary" : metrics table
%   Sheet "fixed"   : t, Pr, P, e, fs, d_feas (if exists), U1, U2
%   Sheet "old"     : same
%   Sheet "base"    : same
%
% Requirements:
%   - logsout contains at least: Pr, P, fs, U1, U2
%   - duty signal logged as d_feas (recommended). If not, add its name to dutyNames.

%% ===== config =====
mdl   = 'VVA_pressure_dynamic_basefreq';
stopT = '0.6';

modes  = [1 2 3];            % 1=fixed,2=old,3=base
labels = {'fixed','old','base'};

settleBand = 0.01;           % 1% settling band
uThr = 0.5;

dutyNames = {'d_feas','d','D','d_raw','d_cmd','duty','PWM_duty'};

%% ===== run =====
res = struct([]);
for i = 1:numel(modes)
    resetPersistents();
    assignin('base','freq_mode',modes(i));

    simOut = sim(mdl,'StopTime',stopT);
    logs = simOut.logsout;

    % Required signals
    [tP, P]   = getTS_noWarn(logs,'P');
    [tR, Pr]  = getTS_noWarn(logs,'Pr');
    [tfs, fs] = getTS_noWarn(logs,'fs');
    [t1, U1]  = getTS_noWarn(logs,'U1');
    [t2, U2]  = getTS_noWarn(logs,'U2');

    % Common time base
    if ~isempty(tP)
        t = tP(:);
    elseif ~isempty(tR)
        t = tR(:);
    else
        t = simOut.tout(:);
    end

    % Interp (analog: linear, digital: previous)
    P  = interpTo(tP,  P,  t, 'linear');
    Pr = interpTo(tR,  Pr, t, 'linear');
    fs = interpTo(tfs, fs, t, 'linear');
    U1 = interpTo(t1,  U1, t, 'previous');
    U2 = interpTo(t2,  U2, t, 'previous');

    % Optional duty
    d = []; d_name = '';
    for k = 1:numel(dutyNames)
        [td, dv] = getTS_noWarn(logs, dutyNames{k});
        if ~isempty(dv)
            d = interpTo(td, dv, t, 'linear');
            d_name = dutyNames{k};
            break;
        end
    end

    % Error
    e = Pr - P;
    ref = Pr(end);

    % Metrics
    RMSE = sqrt(mean(e.^2));
    IAE  = trapz(t, abs(e));
    ISE  = trapz(t, e.^2);

    ess_abs = abs(P(end) - ref);
    overshoot = max(P) - ref;
    Mp = overshoot / max(abs(ref), eps);

    tr = riseTime(t, P, ref, 0.1, 0.9);
    ts = settleTime(t, P, ref, settleBand);

    N1 = countToggles(U1, uThr);
    N2 = countToggles(U2, uThr);
    Nsw = N1 + N2;

    Tsim = t(end) - t(1);
    sw_rate = Nsw / max(Tsim, eps);

    fmean = mean(fs);
    fmin  = min(fs);
    fmax  = max(fs);

    % Store
    res(i).lbl = labels{i};
    res(i).mode = modes(i);
    res(i).t = t;
    res(i).Pr = Pr; res(i).P = P; res(i).e = e;
    res(i).fs = fs;
    res(i).U1 = U1; res(i).U2 = U2;
    res(i).d  = d;  res(i).d_name = d_name;

    res(i).RMSE = RMSE; res(i).IAE = IAE; res(i).ISE = ISE;
    res(i).ess_abs = ess_abs; res(i).Mp = Mp;
    res(i).tr = tr; res(i).ts = ts;
    res(i).N1 = N1; res(i).N2 = N2; res(i).Nsw = Nsw; res(i).sw_rate = sw_rate;
    res(i).fmean = fmean; res(i).fmin = fmin; res(i).fmax = fmax;
end

%% ===== summary table =====
T = table( ...
    string({res.lbl})', ...
    [res.RMSE]', [res.IAE]', [res.ISE]', ...
    [res.ess_abs]', [res.Mp]', [res.tr]', [res.ts]', ...
    [res.N1]', [res.N2]', [res.Nsw]', [res.sw_rate]', ...
    [res.fmean]', [res.fmin]', [res.fmax]', ...
    'VariableNames', {'mode','RMSE','IAE','ISE','ess_abs','Mp','tr','ts', ...
                      'N1','N2','Nsw','sw_rate','fmean','fmin','fmax'} );
disp(T);

%% ===== export to Excel =====
tsTag = datestr(now,'yyyymmdd_HHMMSS');
xlsxFile = ['compare_export_' tsTag '.xlsx'];

% 1) summary sheet
writetable(T, xlsxFile, 'Sheet','summary');

% 2) per-mode time series sheets
for i = 1:numel(res)
    dn = res(i).d_name;
    if isempty(dn), dn = 'duty'; end

    if isempty(res(i).d)
        TT = table(res(i).t, res(i).Pr, res(i).P, res(i).e, res(i).fs, res(i).U1, res(i).U2, ...
            'VariableNames', {'t','Pr','P','e','fs','U1','U2'});
    else
        TT = table(res(i).t, res(i).Pr, res(i).P, res(i).e, res(i).fs, res(i).d, res(i).U1, res(i).U2, ...
            'VariableNames', {'t','Pr','P','e','fs',dn,'U1','U2'});
    end

    writetable(TT, xlsxFile, 'Sheet', res(i).lbl);
end

fprintf('Excel exported: %s\n', xlsxFile);

end

%% ===== helpers =====
function resetPersistents()
clear VarFreqPWM BaseFreqOpt FreqPolicy frequency Duty_cycle_compensation DutyComp_D
end

function [t, x] = getTS_noWarn(logs, name)
t = []; x = [];
try
    names = logs.getElementNames;
    if isstring(names), names = cellstr(names); end
    if ~any(strcmp(names, name)), return; end
    el = logs.getElement(name);
    ts = el.Values;
    t  = ts.Time(:);
    x  = ts.Data(:);
catch
end
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
b = u(:) > thr;
N = sum(diff(b) ~= 0);
end

function ts = settleTime(t, y, ref, band)
if isempty(t) || isempty(y), ts = NaN; return; end
th = band * abs(ref);
inside = abs(y - ref) <= th;
viol = ~inside;
suf = flipud(cummax(flipud(viol)));
idx = find(suf==0, 1, 'first');
if isempty(idx), ts = NaN; else, ts = t(idx); end
end

function tr = riseTime(t, y, ref, p1, p2)
if isempty(t) || isempty(y) || ref==0, tr = NaN; return; end
y1 = p1*ref; y2 = p2*ref;
i1 = find(y >= y1, 1, 'first');
i2 = find(y >= y2, 1, 'first');
if isempty(i1) || isempty(i2) || i2<=i1, tr = NaN; else, tr = t(i2)-t(i1); end
end
