function [T, res] = new_run_compare()
% Compare fixed/old/base with richer plots + metrics (no noisy warnings)
%
% Must have logsout signals at least: Pr, P, fs, U1, U2
% Duty is optional: any of d_feas / d / D / d_raw / d_cmd etc.

%% ===== config =====
mdl   = 'VVA_pressure_dynamic_basefreq';
stopT = '0.6';

modes  = [1 2 3];            % 1=fixed,2=old,3=base
labels = {'fixed','old','base'};

settleBand = 0.01;           % 1% settling band
uThr = 0.5;                  % toggle threshold
Ts_assume = 1e-4;            % only used if time missing (rare)

% Duty candidates: add your true logged name here if different
dutyNames = {'d_feas','d','D','d_raw','d_cmd','duty','PWM_duty'};

%% ===== run =====
res = struct([]);
for i = 1:numel(modes)
    resetPersistents();                  % important for fair comparison
    assignin('base','freq_mode',modes(i));

    simOut = sim(mdl,'StopTime',stopT);
    logs = simOut.logsout;

    % List what is actually in logsout (helpful once)
    if i == 1
        names = getElementNamesSafe(logs);
        fprintf('logsout signals (%d): %s\n', numel(names), strjoin(names, ', '));
    end

    % --- required signals ---
    [tP, P]   = getTS_noWarn(logs,'P');
    [tR, Pr]  = getTS_noWarn(logs,'Pr');
    [tfs, fs] = getTS_noWarn(logs,'fs');
    [t1, U1]  = getTS_noWarn(logs,'U1');
    [t2, U2]  = getTS_noWarn(logs,'U2');

    % pick a common time base
    if ~isempty(tP)
        t = tP(:);
    elseif ~isempty(tR)
        t = tR(:);
    elseif ~isempty(simOut.tout)
        t = simOut.tout(:);
    else
        t = (0:Ts_assume:str2double(stopT)).';
    end

    % interpolate (analog: linear, digital: previous)
    P  = interpTo(tP,  P,  t, 'linear');
    Pr = interpTo(tR,  Pr, t, 'linear');
    fs = interpTo(tfs, fs, t, 'linear');

    U1 = interpTo(t1,  U1, t, 'previous');
    U2 = interpTo(t2,  U2, t, 'previous');

    % --- optional duty ---
    d = []; d_name = '';
    for k = 1:numel(dutyNames)
        [td, dv] = getTS_noWarn(logs, dutyNames{k});
        if ~isempty(dv)
            d = interpTo(td, dv, t, 'linear');
            d_name = dutyNames{k};
            break;
        end
    end

    % --- error ---
    e = Pr - P;
    ref = Pr(end);

    % --- metrics ---
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

    % store
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

%% ===== table =====
T = table( ...
    string({res.lbl})', ...
    [res.RMSE]', [res.IAE]', [res.ISE]', ...
    [res.ess_abs]', [res.Mp]', [res.tr]', [res.ts]', ...
    [res.N1]', [res.N2]', [res.Nsw]', [res.sw_rate]', ...
    [res.fmean]', [res.fmin]', [res.fmax]', ...
    'VariableNames', {'mode','RMSE','IAE','ISE','ess_abs','Mp','tr','ts', ...
                      'N1','N2','Nsw','sw_rate','fmean','fmin','fmax'} );
disp(T);

%% ===== plots =====
figure('Name','Compare: Pressure/Frequency/Error/Duty/U1/U2');
tl = tiledlayout(3,2,'TileSpacing','compact','Padding','compact');

% Pressure
nexttile; hold on; grid on;
plot(res(1).t, res(1).Pr, 'k--', 'DisplayName','Reference');
for i=1:numel(res), plot(res(i).t, res(i).P, 'DisplayName',[res(i).lbl ' P']); end
xlabel('Time (s)'); ylabel('Pressure (Pa)'); title('Pressure Tracking');
legend('show','Location','best');

% Error
nexttile; hold on; grid on;
for i=1:numel(res), plot(res(i).t, res(i).e, 'DisplayName',[res(i).lbl ' e=Pr-P']); end
xlabel('Time (s)'); ylabel('Error (Pa)'); title('Tracking Error');
legend('show','Location','best');

% Frequency
nexttile; hold on; grid on;
for i=1:numel(res), plot(res(i).t, res(i).fs, 'DisplayName',[res(i).lbl ' fs']); end
xlabel('Time (s)'); ylabel('Frequency (Hz)'); title('PWM Base Frequency');
legend('show','Location','best');

% Duty
nexttile; hold on; grid on;
hasDuty = false;
for i=1:numel(res)
    if ~isempty(res(i).d)
        hasDuty = true;
        dn = res(i).d_name; if isempty(dn), dn = 'd'; end
        plot(res(i).t, res(i).d, 'DisplayName',[res(i).lbl ' ' dn]);
    end
end
xlabel('Time (s)'); ylabel('Duty'); title('Duty Signal (logged)');
legend('show','Location','best');
if ~hasDuty
    text(0.05,0.6, 'Duty not found in logsout.', 'Units','normalized');
    text(0.05,0.45,'Please enable Signal Logging for d_feas (or add its name to dutyNames).', 'Units','normalized');
end

% U1 (digital) - use stairs + y-lim so 0 line is visible
nexttile; hold on; grid on;
for i=1:numel(res), stairs(res(i).t, res(i).U1, 'DisplayName',[res(i).lbl ' U1']); end
xlabel('Time (s)'); ylabel('U1'); title('Valve Drive U1 (digital)');
ylim([-0.1 1.1]); yticks([0 1]);
legend('show','Location','best');

% --- U2 (digital) ---
nexttile; hold on; grid on;

% baseline zero (so you can see where "all-zero" sits)
plot([res(1).t(1) res(1).t(end)], [0 0], 'k:', 'DisplayName','U2=0 baseline');

for i=1:numel(res)
    stairs(res(i).t, res(i).U2, 'LineWidth', 1.5, ...
        'DisplayName',[res(i).lbl ' U2']);
end
xlabel('Time (s)'); ylabel('U2'); title('Valve Drive U2 (digital)');
ylim([-0.1 1.1]); yticks([0 1]);
legend('show','Location','best');

% Cumulative toggles (still useful)
figure('Name','Compare: Cumulative Switching Count');
hold on; grid on;
for i=1:numel(res)
    cumN = cumulativeToggles(res(i).U1, uThr) + cumulativeToggles(res(i).U2, uThr);
    plot(res(i).t, [0; cumN], 'DisplayName',[res(i).lbl ' cumN']);
end
xlabel('Time (s)'); ylabel('Cumulative toggles'); title('Cumulative Switching Count');
legend('show','Location','best');

end

%% ===== helpers =====
function resetPersistents()
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
if ~any(strcmp(names, name))
    return;
end
el = logs.getElement(name);
ts = el.Values;              % timeseries
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
b = u(:) > thr;
N = sum(diff(b) ~= 0);
end

function cumN = cumulativeToggles(u, thr)
if isempty(u), cumN = []; return; end
b = u(:) > thr;
cumN = cumsum(diff(b) ~= 0);
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
