%% run_ga_export_pareto.m
% Fast multi-objective GA (gamultiobj) -> Pareto scatter + export + knee/min-distance
% 适配：你的 Simulink Lookup Table 可能依赖 base workspace 的 bp/y/z（或 x）
% 关键：每次评估 objfun_ga 之前，把这些“查表变量”恢复回去，避免被脚本/GA污染

clc;

%% ---------------- user settings ----------------
mdl   = 'VVA_pressure_dynamic_basefreq_sin_GA';   % 模型名（和 objfun_ga.m 里保持一致）
nvars = 3;                                       % [kf, f_min, lambda_lp]

lb    = [0.6,  80,  0.05];
ub    = [1.2, 300,  0.98];

% 为了更快：把迭代次数和种群调小
popSize = 24;        % 越小越快，但 Pareto 点会少
maxGen  = 6;         % 越小越快
maxStallGen = 3;     % 停滞几代就提前结束（加速）

bad_th  = 1e11;      % 判定 penalty 的阈值（objfun_ga 失败返回 1e12）
maxTriesFactor = 20; % 初始可行个体采样强度：越大越稳但越慢（这里取小更快）
precheckN = 2;       % 预检查随机点个数（小一点更快）

% 是否实时画 GA 内置 pareto（一般会拖慢）
showGaplot = false;

%% ---------------- ensure model loaded ----------------
try
    if ~bdIsLoaded(mdl)
        load_system(mdl);
    end
catch ME
    error('无法加载模型 %s ：%s', mdl, ME.message);
end

%% ---------------- capture base workspace map variables ----------------
% 你的截图里 Lookup Table 使用的是 bp/y/z；你旧脚本里提到 x。
% 这里统一做：如果 base 里存在这些变量，就抓一份备份，每次 objfun 前恢复。
preserveList = {'bp','y','z','x'};   % 如模型里还有别的查表变量，直接加到这个列表里
baseVars = struct();
foundAny = false;

for i = 1:numel(preserveList)
    v = preserveList{i};
    if evalin('base', sprintf('exist(''%s'',''var'')', v))
        baseVars.(v) = evalin('base', v);
        foundAny = true;
    end
end

if ~foundAny
    warning(['base workspace 里没有找到 bp/y/z/x 任何一个变量。' ...
             '如果你的 Lookup Table 确实依赖这些变量，请先把它们放进 base workspace。']);
else
    fprintf('Captured base map vars: ');
    disp(fieldnames(baseVars).');
end

%% ---------------- objective wrapper (restore maps before each eval) ----------------
obj = @(dv) objfun_restore_basevars(dv, baseVars);

%% ---------------- quick precheck ----------------
fprintf('=== Precheck: sample %d points ===\n', precheckN);
for i = 1:precheckN
    dv_cand = lb + (ub-lb).*rand(1,nvars);
    F = obj(dv_cand);
    fprintf('cand %d: [%.3f %.1f %.3f] -> [%.6g %.0f]\n', ...
        i, dv_cand(1), dv_cand(2), dv_cand(3), F(1), F(2));
end

%% ---------------- build feasible initial population (faster version) ----------------
fprintf('=== Building feasible initial population (size=%d) ===\n', popSize);
initPop = buildFeasibleInitPop(popSize, nvars, lb, ub, obj, bad_th, maxTriesFactor);
fprintf('=== Init population ready ===\n\n');

%% ---------------- GA options ----------------
gaOpts = optimoptions('gamultiobj', ...
    'PopulationSize', popSize, ...
    'MaxGenerations', maxGen, ...
    'MaxStallGenerations', maxStallGen, ...
    'CrossoverFraction', 0.8, ...
    'FunctionTolerance', 1e-4, ...
    'UseParallel', false, ...          % 并行有时更快，但开销也可能更大；先关保证稳定
    'Display', 'iter', ...
    'InitialPopulationMatrix', initPop);

if showGaplot
    gaOpts = optimoptions(gaOpts, 'PlotFcn', {@gaplotpareto});
else
    gaOpts = optimoptions(gaOpts, 'PlotFcn', []);
end

fprintf('>>> Running gamultiobj: Pop=%d, MaxGen=%d, MaxStallGen=%d\n\n', ...
    gaOpts.PopulationSize, gaOpts.MaxGenerations, gaOpts.MaxStallGenerations);

%% ---------------- run gamultiobj ----------------
[Xpareto, Fpareto, exitflag, output] = ...
    gamultiobj(obj, nvars, [], [], [], [], lb, ub, gaOpts);

%% ---------------- filter penalty points ----------------
isBad = any(~isfinite(Fpareto),2) | any(Fpareto >= bad_th,2);
Xok   = Xpareto(~isBad,:);
Fok   = Fpareto(~isBad,:);

fprintf('\n=== Done. Raw Pareto=%d, Filtered Pareto=%d (removed penalty=%d) ===\n', ...
    size(Fpareto,1), size(Fok,1), sum(isBad));

if isempty(Fok)
    warning(['过滤后无有效 Pareto 点（全是 penalty）。' ...
             '请优先检查：1) base 的 bp/y/z 是否被清掉；2) simOut 是否能取到 Pr/P/U1/U2；3) 模型是否报错。']);
    return;
end

%% ---------------- pick compromise points ----------------
% 1) min-distance：min-max 归一化后距离(0,0)最小
Fnorm = minmaxNorm(Fok);
D = sqrt(sum(Fnorm.^2, 2));
[~, idx_star] = min(D);

x_star = Xok(idx_star,:);
f_star = Fok(idx_star,:);

% 2) knee 点：对 (J1,J2) 排序，找最大到端点连线垂距
idx_knee = kneePointIndex(Fok);
x_knee = Xok(idx_knee,:);
f_knee = Fok(idx_knee,:);

fprintf('Min-distance idx=%d  x*=[%.4f %.2f %.4f], F*=[%.6g %.0f]\n', ...
    idx_star, x_star(1), x_star(2), x_star(3), f_star(1), f_star(2));
fprintf('Knee-point   idx=%d  xk=[%.4f %.2f %.4f], Fk=[%.6g %.0f]\n', ...
    idx_knee, x_knee(1), x_knee(2), x_knee(3), f_knee(1), f_knee(2));

%% ---------------- export ----------------
T = table(Xok(:,1), Xok(:,2), Xok(:,3), Fok(:,1), Fok(:,2), ...
    'VariableNames', {'kf','f_min','lambda_lp','J1_nRMSE','J2_Nsw'});

writetable(T, 'pareto_points_filtered.csv');
save('pareto_points_filtered.mat', 'Xok','Fok','T','x_star','f_star','x_knee','f_knee','exitflag','output');

Tstar = table(x_star(1),x_star(2),x_star(3),f_star(1),f_star(2), ...
    'VariableNames', {'kf','f_min','lambda_lp','J1_nRMSE','J2_Nsw'});
Tknee = table(x_knee(1),x_knee(2),x_knee(3),f_knee(1),f_knee(2), ...
    'VariableNames', {'kf','f_min','lambda_lp','J1_nRMSE','J2_Nsw'});

writetable(Tstar,'compromise_min_distance.csv');
writetable(Tknee,'compromise_knee.csv');

%% ---------------- plot pareto scatter ----------------
figure('Name','Pareto Front (Filtered)');
scatter(Fok(:,1), Fok(:,2), 28, 'filled'); grid on; box on;
xlabel('J1: normalized RMSE');
ylabel('J2: switching count N_{sw}');
title('Pareto Front (penalty removed)');

hold on;
scatter(f_star(1), f_star(2), 90, 'filled');
text(f_star(1), f_star(2), '  Min-distance', 'VerticalAlignment','bottom');

scatter(f_knee(1), f_knee(2), 90, 'filled');
text(f_knee(1), f_knee(2), '  Knee', 'VerticalAlignment','top');

legend({'Pareto points','Min-distance','Knee'}, 'Location','best');
hold off;

savefig('pareto_front_filtered.fig');
saveas(gcf,'pareto_front_filtered.png');

fprintf('\nSaved:\n  pareto_front_filtered.png/.fig\n  pareto_points_filtered.csv/.mat\n  compromise_min_distance.csv\n  compromise_knee.csv\n');

%% =================== local functions ===================

function F = objfun_restore_basevars(dv, baseVars)
% 每次评估前恢复 base workspace 的查表变量（bp/y/z/x...）
    try
        if ~isempty(baseVars)
            fns = fieldnames(baseVars);
            for k = 1:numel(fns)
                assignin('base', fns{k}, baseVars.(fns{k}));
            end
        end
        F = objfun_ga(dv);
    catch
        F = [1e12 1e12];
    end
end

function initPop = buildFeasibleInitPop(N, nvars, lb, ub, obj, bad_th, maxTriesFactor)
% 更快版本：尽量找可行点，找不到就随机填
    initPop = zeros(N, nvars);
    cnt = 0;
    tries = 0;
    maxTries = maxTriesFactor * N;

    while cnt < N && tries < maxTries
        tries = tries + 1;
        dv = lb + (ub-lb).*rand(1,nvars);
        F = obj(dv);

        if all(isfinite(F)) && all(F < bad_th)
            cnt = cnt + 1;
            initPop(cnt,:) = dv;
        end
    end

    if cnt < N
        warning('只收集到 %d/%d 个可行初始个体（剩下用随机填充）。', cnt, N);
        for k = cnt+1:N
            initPop(k,:) = lb + (ub-lb).*rand(1,nvars);
        end
    end
end

function Fn = minmaxNorm(F)
% min-max 归一化到 [0,1]
    fmin = min(F,[],1);
    fmax = max(F,[],1);
    frng = fmax - fmin;
    frng(frng < eps) = 1;
    Fn = (F - fmin) ./ frng;
end

function idx = kneePointIndex(F)
% 简单 knee：按 J1 排序，找最大到端点连线的垂距
    if size(F,1) <= 2
        idx = 1;
        return;
    end

    [~, ord] = sort(F(:,1), 'ascend');
    Fs = F(ord,:);

    A = Fs(1,:);
    B = Fs(end,:);
    v = B - A;
    nv = norm(v);
    if nv < eps
        idx = ord(1);
        return;
    end
    v = v / nv;

    d = zeros(size(Fs,1),1);
    for i = 1:size(Fs,1)
        w = Fs(i,:) - A;
        proj = dot(w,v)*v;
        perp = w - proj;
        d(i) = norm(perp);
    end

    [~, imax] = max(d);
    idx = ord(imax);
end
