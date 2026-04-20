# esp32claw

<div align="center">

### 在 ESP32-S3 上部署一个轻量、常驻、可执行工具的 AI 助手

<p>
  <strong><a href="README.md">中文</a></strong> ·
  <strong><a href="README_EN.md">English</a></strong>
</p>

</div>

> 基于上游 [MimiClaw](https://github.com/memovai/mimiclaw) 的二次开发项目。  
> `esp32claw` 运行在低成本 ESP32-S3 开发板上，支持飞书、Telegram、WebSocket、多工具调用、GPIO 控制、定时任务、心跳唤醒、SPIFFS 记忆，以及 WS2812B 呼吸灯。

## 项目亮点

| 特性 | 说明 |
|------|------|
| 轻量运行 | 无 Linux、无 Node.js、纯 C 实现，适合长期在线 |
| 多通道交互 | 支持飞书、Telegram、WebSocket |
| 工具执行 | 支持文件、GPIO、LED、搜索、时间、设备状态、Cron 等工具 |
| 持久记忆 | 记忆、会话、技能、任务全部保存在 SPIFFS |
| 低功耗 | 适合 USB 或电池供电场景 |
| 可玩性强 | 一块 ESP32-S3 板子就能跑出完整 Agent 体验 |

## 认识 esp32claw

- **小巧**：没有 Linux，没有 Node.js，没有臃肿依赖
- **直接**：发一条消息，它就能调用工具去执行
- **持久**：记忆、会话和任务都能跨重启保留
- **常驻**：低功耗、低成本，适合 24/7 在线
- **好玩**：能聊天，也能亮灯、读写文件、跑任务

## 相比上游 MimiClaw 的主要改动

- **接入 MiniMax API**，降低日常使用成本
- **新增 WS2812B 驱动**，支持呼吸灯效果
- **新增设备状态查询工具**，可查看 Wi-Fi、IP、运行时长、内存、PSRAM、灯光状态等
- **优化 Wi-Fi 重连逻辑**，减少异常断连后的卡死风险
- **修复仅配置飞书时 Telegram 分支仍运行** 的问题
- **优化联网后本地管理 AP 常驻** 的问题，减少资源占用

## 快速导航

- [快速开始](#快速开始)
- [CLI 命令](#cli-命令通过-uartcom-口连接)
- [记忆](#记忆)
- [工具](#工具)
- [定时任务（Cron）](#定时任务cron)
- [心跳（Heartbeat）](#心跳heartbeat)
- [其他功能](#其他功能)
- [许可证](#许可证)

## 快速开始

### 你需要

- 一块 **ESP32-S3 开发板**，16MB Flash + 8MB PSRAM
- 一根 **USB Type-C 数据线**
- 一个 **飞书机器人**
- 一个 **大模型 API**

### Ubuntu 和 macOS

参照上游 [MimiClaw](https://github.com/memovai/mimiclaw) 的安装说明即可。

### Windows 11

```bash
# 需要先安装 ESP-IDF v5.5+
# 若 git clone 下载不下来，可以到官网手动下载 v5.5.2 zip 包
git clone -b v5.5.2 --recursive https://github.com/espressif/esp-idf.git

git clone https://github.com/zhangjun2636808827/esp32claw

# 先安装 idf
cd esp-idf-v5.5.2

# 终端进入 cmd，不然安装后可能识别不到 idf.py
cmd

# 安装
install.bat
export.bat

# 验证安装
idf.py --help
```

<img src="assets/install.png" alt="ESP-IDF 安装" width="1080" />

```bash
# 进入 esp32claw 目录
cd esp32claw

# 选择芯片
idf.py set-target esp32s3

# 复制配置文件，所有信息都填写在这个文件中
cp main/mimi_secrets.h.example main/mimi_secrets.h
```

<img src="assets/config.png" alt="项目配置" width="1080" />

### 获取 [飞书](https://open.feishu.cn/app) 信息

<img src="assets/feishu.png" alt="飞书配置 1" width="1080" />
<img src="assets/feishu2.png" alt="飞书配置 2" width="1080" />

### 复制 App ID 和 App Secret 到 `MIMI_SECRET_FEISHU_APP_ID` 与 `MIMI_SECRET_FEISHU_APP_SECRET`

<img src="assets/feishu3.png" alt="飞书 App 凭据" width="1080" />

### 创建机器人

<img src="assets/feishu4.png" alt="创建飞书机器人" width="1080" />

### 配置机器人权限

<img src="assets/feishu5.png" alt="配置飞书权限" width="1080" />

```json
{
  "scopes": {
    "tenant": [
      "aily:file:read",
      "aily:file:write",
      "application:application.app_message_stats.overview:readonly",
      "application:application:self_manage",
      "application:bot.menu:write",
      "cardkit:card:read",
      "cardkit:card:write",
      "contact:user.employee_id:readonly",
      "corehr:file:download",
      "event:ip_list",
      "im:chat.access_event.bot_p2p_chat:read",
      "im:chat.members:bot_access",
      "im:message",
      "im:message.group_at_msg:readonly",
      "im:message.p2p_msg:readonly",
      "im:message:readonly",
      "im:message:send_as_bot",
      "im:resource"
    ],
    "user": [
      "aily:file:read",
      "aily:file:write",
      "im:chat.access_event.bot_p2p_chat:read"
    ]
  }
}
```

<img src="assets/feishu6.png" alt="飞书权限 JSON" width="1080" />

### 开启以下权限

<img src="assets/feishu7.png" alt="开启飞书权限" width="1080" />

### 配置事件订阅

<img src="assets/feishu8.png" alt="配置事件订阅" width="1080" />

### 发布应用

<img src="assets/feishu9.png" alt="发布飞书应用 1" width="1080" />
<img src="assets/feishu10.png" alt="发布飞书应用 2" width="1080" />

### 在飞书里测试机器人

飞书创建一个群聊，然后点击 `设置 -> 群机器人 -> 添加`，选择你前面创建的机器人。  
在群里 `@esp32claw` 并发送消息，例如：

`@esp32claw 帮我打开蓝色呼吸灯，周期为 2s`

<img src="assets/feishu11.png" alt="飞书中使用机器人" width="240" />

### 配置 [MiniMax](https://www.minimaxi.com/) 信息

- 订阅一个 [MiniMax](https://www.minimaxi.com/) 的 29 RMB Starter 套餐
- 进入 `账户管理 -> Token Plan`
- 复制 Token Plan Key 到 `MIMI_SECRET_API_KEY`

<img src="assets/MiniMax1.png" alt="MiniMax Token Plan Key" width="1080" />

```c
#define MIMI_SECRET_MODEL           "MiniMax-M2.7" // 模型名称，详见 https://minimaxi.com/docs/models
#define MIMI_SECRET_MODEL_PROVIDER  "openai"       // 提供者；默认为 Anthropic，使用 MiniMax 时请设置为 "openai"
```

### 完成 `mimi_secrets.h` 后，配置 WS2812B

打开 `main/mimi_config.h` 文件：

<img src="assets/image8.png" alt="WS2812B 配置" width="1080" />

### 编译并烧写固件

### ESP32 开发板连接电脑，注意接上烧录口

<img src="assets/image.png" alt="连接 ESP32 开发板" width="1080" />

### 打开设备管理器

<img src="assets/image2.png" alt="设备管理器" width="240" />

### 查看烧录口对应的端口号

<img src="assets/image3.png" alt="查看 COM 口" width="1080" />

### 例如端口为 `COM5`，接下来开始编译

```bash
# 完整编译（修改 mimi_secrets.h 后建议 fullclean）
idf.py fullclean && idf.py build

idf.py -p COM5 flash monitor
```

<img src="assets/image4.png" alt="编译输出" width="1080" />
<img src="assets/image5.png" alt="烧录输出" width="1080" />

### 出现以下界面表示编译和烧录成功

<img src="assets/image6.png" alt="烧录成功 1" width="1080" />
<img src="assets/image7.png" alt="烧录成功 2" width="1080" />

### 连接串口并打开 MobaXterm

将 USB 线接到串口，打开 `MobaXterm/MobaXterm.exe`，按以下步骤打开串口：

<img src="assets/image9.png" alt="打开串口 1" width="1080" />
<img src="assets/image10.png" alt="打开串口 2" width="1080" />

### 按下 ESP32 开发板上的复位按键

<img src="assets/image11.png" alt="复位开发板" width="1080" />

### 现在开始聊天

<img src="assets/image12.jpg" alt="聊天示例 1" width="240" />
<img src="assets/image13.jpg" alt="聊天示例 2" width="240" />
<img src="assets/image14.png" alt="聊天示例 3" width="480" />

### CLI 命令（通过 UART/COM 口连接）

通过串口连接即可配置和调试。**配置命令**让你无需重新编译就能修改设置，随时插上 USB 线就能改。

**运行时配置**（存入 NVS，覆盖编译时默认值）：

```text
mimi> wifi_set MySSID MyPassword   # 换 WiFi
mimi> set_tg_token 123456:ABC...   # 换 Telegram Bot Token
mimi> set_api_key sk-ant-api03-... # 换 API Key（Anthropic 或 OpenAI）
mimi> set_model_provider openai    # 切换提供商（anthropic|openai）
mimi> set_model gpt-4o             # 换模型
mimi> set_proxy 192.168.1.83 7897  # 设置代理
mimi> clear_proxy                  # 清除代理
mimi> set_search_key BSA...        # 设置 Brave Search API Key
mimi> set_tavily_key tvly-...      # 设置 Tavily API Key（优先）
mimi> config_show                  # 查看所有配置（脱敏显示）
mimi> config_reset                 # 清除 NVS，恢复编译时默认值
```

**调试与运维：**

```text
mimi> wifi_status              # 连上了吗？
mimi> memory_read              # 看看它记住了什么
mimi> memory_write "内容"       # 写入 MEMORY.md
mimi> heap_info                # 还剩多少内存？
mimi> session_list             # 列出所有会话
mimi> session_clear 12345      # 删除一个会话
mimi> heartbeat_trigger        # 手动触发一次心跳检查
mimi> cron_start               # 立即启动 cron 调度器
mimi> restart                  # 重启
```

## 记忆

esp32claw 把所有数据存为纯文本文件，可以直接读取和编辑：

| 文件 | 说明 |
|------|------|
| `SOUL.md` | 机器人的人设，编辑它来改变行为方式 |
| `USER.md` | 关于你的信息，如姓名、偏好、语言 |
| `MEMORY.md` | 长期记忆，它应该一直记住的事 |
| `HEARTBEAT.md` | 待办清单，机器人定期检查并自主执行 |
| `cron.json` | 定时任务，AI 创建的周期性或一次性任务 |
| `2026-02-05.md` | 每日笔记，直接存放在 `/spiffs/memory/` 下 |
| `tg_12345.jsonl` | 聊天记录，当前实现统一存放在 `/spiffs/sessions/` 下，文件名格式为 `tg_<chat_id>.jsonl` |

## 工具

esp32claw 同时支持 Anthropic 和 OpenAI 的工具调用。LLM 在对话中可以调用工具，循环执行直到任务完成，也就是典型的 ReAct 模式。

| 工具 | 说明 |
|------|------|
| `web_search` | 通过 Tavily（优先）或 Brave 搜索网页，获取实时信息 |
| `get_current_time` | 通过 HTTP 获取当前日期和时间，并设置系统时钟 |
| `get_device_status` | 查询设备当前 Wi-Fi、IP、运行时长、内存、PSRAM 和灯光状态 |
| `read_file` / `write_file` / `edit_file` / `list_dir` | 读写和列出 SPIFFS 文件 |
| `cron_add` | 创建定时或一次性任务，LLM 可自主创建 cron 任务 |
| `cron_list` | 列出所有已调度的 cron 任务 |
| `cron_remove` | 按 ID 删除 cron 任务 |
| `gpio_write` / `gpio_read` / `gpio_read_all` | 控制和读取允许访问的 GPIO |
| `set_led_color` / `led_off` / `breathing_led_on` / `breathing_led_off` / `breathing_led_status` | 控制 GPIO48 上的 WS2812B 灯珠 |

启用网页搜索可在 `mimi_secrets.h` 中设置 [Tavily API key](https://app.tavily.com/home)（优先，`MIMI_SECRET_TAVILY_KEY`），或 [Brave Search API key](https://brave.com/search/api/)（`MIMI_SECRET_SEARCH_KEY`）。

## 定时任务（Cron）

esp32claw 内置 cron 调度器，让 AI 可以自主安排任务。LLM 可以通过 `cron_add` 工具创建周期性任务，例如“每 N 秒”，也可以创建一次性任务，例如“在某个时间戳触发”。任务触发时，消息会注入到 Agent 循环，由 AI 自动醒来、处理并回复。

任务持久化存储在 SPIFFS 的 `cron.json` 中，重启后不会丢失。典型用途包括每日总结、定时提醒和定期巡检。

## 心跳（Heartbeat）

心跳服务会定期读取 SPIFFS 上的 `HEARTBEAT.md`，检查是否存在待办事项。如果发现未完成的条目，例如非空行、非标题、非 `- [x]` 的内容，就会向 Agent 循环发送提示，让 AI 自主处理。

这让 esp32claw 变成一个主动型助理。你只要把任务写入 `HEARTBEAT.md`，机器人就会在下一次心跳周期自动拾取执行，默认周期为每 30 分钟。

## 其他功能

- **WebSocket 网关**：端口 `18789`，局域网内可用任意 WebSocket 客户端连接
- **双核处理**：网络 I/O 和 AI 处理分别跑在不同 CPU 核心
- **HTTP 代理**：支持 CONNECT 隧道，适配受限网络
- **SOCKS5 代理**：支持可选 SOCKS5 隧道
- **多提供商**：同时支持 Anthropic（Claude）和 OpenAI（GPT），运行时可切换
- **定时任务**：AI 可自主创建周期性和一次性任务，重启后持久保存
- **心跳服务**：定期检查任务文件，驱动 AI 主动执行
- **工具调用**：ReAct Agent 循环，两种提供商均支持工具调用

说明：`main/ota/` 下保留了 OTA 源码，但当前还没有接入活动启动流程，也没有编进 `main/CMakeLists.txt`。

## 许可证

MIT

## 致谢

感谢上游 MimiClaw 的开发者。  
点击进入上游 [MimiClaw](https://github.com/memovai/mimiclaw) 主页。
