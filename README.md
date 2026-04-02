# 高速开关阀 PWM 基础频率动态优化程序

基于 **MATLAB / Simulink** 的气动压力伺服仿真与优化程序，用于研究高速开关阀（HSV）在 PWM 控制下的**基础频率动态优化**问题。项目围绕“在保证压力跟踪性能的前提下减少阀门动作次数”展开，支持：

- 固定频率、旧版变频策略、当前动态基础频率优化策略的对比
- 阶跃工况与正弦跟踪工况仿真
- 频率、占空比、阀驱动信号、累计开关次数等指标导出
- 基于 `gamultiobj` 的多目标参数优化（跟踪误差 + 开关次数）
- Pareto 前沿可视化与折中解导出

---

## 1. 项目内容

本仓库主要包含 3 类内容：

1. **Simulink 模型**  
   用于搭建单腔双阀气动压力伺服系统及 PWM 变频控制模型。

2. **MATLAB 脚本**  
   用于批量运行仿真、统计性能指标、绘图、导出 Excel/CSV，并执行遗传算法优化。

3. **自动生成文件**  
   包括 `slxc`、代码生成目录、`.obj/.c/.h` 等，这些通常是 Simulink 编译或代码生成的结果文件。

---

## 2. 主要功能

### 2.1 三种控制模式对比
脚本中通过 `freq_mode` 切换三种模式：

- `1 = fixed`：固定基础频率
- `2 = old`：旧版变频策略
- `3 = base`：当前基础频率动态优化策略

### 2.2 两类典型工况
- **阶跃响应工况**：用于比较跟踪速度、超调、稳态误差与开关次数
- **正弦跟踪工况**：用于比较周期跟踪误差、平均频率与累计动作次数

### 2.3 多目标优化
使用 `gamultiobj` 对以下参数进行优化：

- `kf`
- `f_min`
- `lambda_lp`

优化目标为：

- `J1`：归一化压力跟踪误差（normalized RMSE）
- `J2`：阀门总开关次数 `Nsw`

最终输出 Pareto 前沿，并给出：

- 最小距离折中解（min-distance）
- 拐点解（knee point）

---

## 3. 目录说明

```text
程序/
├─ VVA_pressure_dynamic_basefreq.slx              # 阶跃工况主模型
├─ VVA_pressure_dynamic_basefreq_sin.slx          # 正弦跟踪对比模型
├─ VVA_pressure_dynamic_basefreq_sin_GA.slx       # GA 优化专用模型
├─ VVA_pressure_f_522.slx                         # 早期/固定频率相关模型
├─ new_run_compare_export_jieyue.m                # 阶跃工况：对比 + 指标统计 + Excel 导出
├─ run_compare_jieyue.m                           # 阶跃工况：对比 + 绘图
├─ new_run_compare_sin_export.m                   # 正弦工况：对比 + 指标统计 + Excel 导出
├─ run_ga_export_pareto.m                         # 多目标 GA 优化、Pareto 导出与绘图
├─ objfun_ga.m                                    # GA 目标函数
├─ Q.m                                            # 查表/映射数据（x, y, z）
├─ VVA_pressure_dynamic_basefreq/                 # 自动生成代码目录
├─ VVA_pressure_f_522.slxc                        # Simulink 缓存文件
└─ VVA_pressure_fix_f_522.slxc                    # Simulink 缓存文件
```

---

## 4. 运行环境

建议环境：

- **MATLAB**：R2023b 或更高版本
- **Simulink**
- **Global Optimization Toolbox**（运行 `gamultiobj` 必需）

说明：

- `VVA_pressure_dynamic_basefreq*.slx` 模型保存版本显示为 **R2023b**。
- `VVA_pressure_f_522.slx` 为较早版本模型，元数据中显示为 **R2018a**。如版本不一致，打开时可能需要模型升级。

---

## 5. 使用前准备

在运行脚本前，建议先完成以下检查：

### 5.1 加载查表数据
项目中的部分模型/脚本可能依赖 base workspace 中的查表变量（如 `x / y / z`，或 `bp / y / z`）。

可先运行：

```matlab
Q
```

如果你的模型实际使用的是 `bp` 而不是 `x`，请根据模型中的 Lookup Table 设置自行补充或修改变量名。

### 5.2 检查日志信号
对比脚本默认从 `logsout` 中读取以下信号：

- `P`：实际压力
- `Pr`：参考压力
- `fs`：PWM 基础频率
- `U1`、`U2`：两路阀驱动信号
- `d_feas`（可选）：占空比或可行占空相关信号

如果你的模型里信号名称不同，需要在脚本中的候选名称列表里同步修改。

### 5.3 清理持久变量
脚本内部已经调用 `resetPersistents()` 来清理部分带 persistent 状态的函数。若你后续新增了频率策略函数，也建议加入清理列表，避免不同模式间相互污染。

---

## 6. 快速开始

### 6.1 阶跃工况对比
生成阶跃工况下三种模式的性能指标与 Excel 数据：

```matlab
[T, res, xlsxFile] = new_run_compare_export()
```

或运行绘图版：

```matlab
[T, res] = new_run_compare()
```

默认模型：

```matlab
VVA_pressure_dynamic_basefreq
```

默认输出指标包括：

- `RMSE`
- `IAE`
- `ISE`
- `ess_abs`
- `Mp`
- `tr`
- `ts`
- `N1 / N2 / Nsw`
- `sw_rate`
- `fmean / fmin / fmax`

### 6.2 正弦跟踪工况对比
运行：

```matlab
[Tmetric, curvesWide] = new_run_compare_sin()
```

默认模型：

```matlab
VVA_pressure_dynamic_basefreq_sin
```

默认输出包括：

- 压力跟踪曲线
- 跟踪误差曲线
- 基础频率曲线
- 占空比曲线（若已记录）
- 阀驱动 `U1/U2`
- 累计开关次数曲线
- Excel 宽表导出（便于 Origin 作图）

### 6.3 多目标遗传算法优化
运行：

```matlab
run_ga_export_pareto
```

默认优化模型：

```matlab
VVA_pressure_dynamic_basefreq_sin_GA
```

优化变量：

```matlab
x = [kf, f_min, lambda_lp]
```

输出结果包括：

- `pareto_points_filtered.csv`
- `pareto_points_filtered.mat`
- `compromise_min_distance.csv`
- `compromise_knee.csv`
- `pareto_front_filtered.fig`
- `pareto_front_filtered.png`

---

## 7. 输出文件说明

### 阶跃工况导出
`new_run_compare_export_jieyue.m` 会导出类似文件：

```text
compare_export_yyyymmdd_HHMMSS.xlsx
```

典型工作表：

- `summary`
- `fixed`
- `old`
- `base`

### 正弦工况导出
`new_run_compare_sin_export.m` 会导出类似文件：

```text
compare_VVA_pressure_dynamic_basefreq_sin_6s.xlsx
```

典型工作表：

- `metrics`
- `curves_wide`

### GA 优化导出
GA 脚本会导出 Pareto 点、折中解与前沿图，便于论文作图与结果复现实验。

---

## 8. 参数说明

### `freq_mode`
用于切换不同频率策略：

```matlab
1 -> fixed
2 -> old
3 -> base
```

### `uThr`
数字阀驱动判定阈值，默认一般为：

```matlab
uThr = 0.5
```

用于统计 `U1/U2` 的开关跳变次数。

### `stopT`
不同脚本设置了不同仿真时长：

- 阶跃工况：`0.6 s`
- 正弦工况：`6 s`
- GA 优化：`1.5 s`

可根据实验需求自行修改。

---

## 9. 已知注意事项

1. **依赖 base workspace 变量**  
   某些 Lookup Table 依赖 `x/y/z` 或 `bp/y/z`，如果变量未提前加载，仿真可能报错。

2. **信号命名需一致**  
   若 `logsout` 中没有 `P/Pr/U1/U2/fs` 等信号，脚本将无法统计指标或直接返回 penalty。

3. **版本兼容问题**  
   不同模型来自不同 MATLAB 版本，首次打开时可能提示升级。

4. **自动生成文件较多**  
   `slxc`、`obj`、生成代码目录通常不是核心源码。若仓库以“论文复现”为目标，建议只保留必要模型、脚本和说明文件。

---

## 10. 建议上传到 GitHub 的内容

建议优先保留：

- `.slx` 模型文件
- `.m` 脚本文件
- `README.md`
- 示例结果图或导出样例（可选）

建议考虑忽略：

- `*.slxc`
- `*.obj`
- 自动生成的 `*.c / *.h / *.bat / *.rsp / *.mk`
- 大型缓存文件和编译中间文件

如果后续整理仓库，推荐再补充一个 `.gitignore`。

---

## 11. 适用场景

该项目适用于以下研究或开发场景：

- 高速开关阀 PWM 控制研究
- 气动压力伺服系统仿真
- 基础频率动态优化策略验证
- 以“跟踪性能—动作次数”折中为目标的多目标优化研究
- 论文结果复现、图表导出与参数敏感性分析

---

## 12. 后续可改进方向

- 增加一键初始化脚本（统一加载变量、打开模型、配置日志）
- 增加批量参数扫描脚本
- 增加实验数据与仿真结果自动对比模块
- 补充 `LICENSE`、`.gitignore` 和结果示例图
- 补充英文版 README，方便国际协作与投稿附录复现

---

## 13. 致谢说明

如果你在论文、项目或二次开发中使用了本仓库，建议在文档中注明其用途，例如：

> 本仓库用于高速开关阀 PWM 基础频率动态优化方法的仿真验证、性能对比与多目标参数优化。

---

## 14. 联系与说明

本仓库当前主要面向研究复现与论文支撑使用，部分变量名、模型命名和脚本命名保留了原始实验阶段习惯。如需发布为公开仓库，建议进一步统一：

- 模型命名
- 变量命名
- 输出文件命名
- 图表导出规范

这样会更适合团队协作与长期维护。
