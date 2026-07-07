# ViaVoicePortable

ViaVoicePortable 是一款轻量级的便携式语音合成（TTS）引擎封装库。

## 运行要求
- **宿主程序**：必须为 32 位（x86）架构。
- **部署文件**：将 `ViaVoiceTTS.dll` 与 `VVTTS_Data.dat` 放在同级目录。

## 一、核心参数与控制接口

> **注**：所有参数取值范围一般为 0 - 100（发音语速 Rate 最高支持 250）。

### 1. 角色设定
- **API**: `VVTTS_SetVoice(id)` / `SetRole(role)`
- **角色映射**:
  - `role=0/1`：男1
  - `role=2`：女1（清脆）
  - `role=3`：童声
  - `role=4`：男2
  - `role=5`：男3
  - `role=6`：女2（温柔）
  - `role=7`：老年女声

### 2. 发音语速
- **API**: `VVTTS_SetRate(val)`
- Rate 建议默认 50，正常上限为 100，但支持最高设定至 250。

### 3. 声音音调
- **API**: `VVTTS_SetPitch(val)`
- Pitch 建议默认 50。

### 4. 发音音量
- **API**: `VVTTS_SetVolume(val)`
- Volume 建议默认 100。

### 5. 生理体型
- **API**: `VVTTS_SetHeadSize(val)`
- HeadSize 改变声道长度，值越小听起来体型越小、年龄越低。

### 6. 声音沙哑
- **API**: `VVTTS_SetRoughness(val)`
- Roughness 表示喉部撕裂感/粗糙度。

### 7. 气声强度
- **API**: `VVTTS_SetBreathiness(val)`

### 8. 播放控制
- **暂停/恢复**: `VVTTS_Pause(1 或 0)`
  - `1` 为暂停，`0` 为恢复；等同于 `Pause()` / `Resume()`。
- **立即中断**: `VVTTS_Stop()`
  - 用于掐断当前正在播放的语音。

### 9. 语音导出与内存处理
- **导出wav文件**: `VVTTS_SpeakToFile`
- **将输出的语音写入内存进行二次处理**: `VVTTS_SpeakToBuffer`

## 二、配置保存与加载

1. **自动保存**：每当有程序调用设置接口（如 `VVTTS_SetRate`、`VVTTS_SetPitch` 等）改变参数时，接口均会自动在宿主程序所在目录下生成并更新 `config.ini` 文件，实时保存语音状态。
2. **配置加载**：宿主程序在初始化引擎（`VVTTS_InitEx`）后，可通过读取该 ini 文件来自动恢复上次的设置选项。
