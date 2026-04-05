# Esp32Claw: 在Esp32s3上部署ai助理


<p align="left">
  <strong><a href="README_EN.md">English</a> | <a href="README.md">中文</a> </strong>
</p>

**本项目基于[MimiClaw](https://github.com/memovai/mimiclaw)，在价值25左右的esp32s3开发板上部署本地AI助手，可以实现飞书控制GPIO、读写本地文件、控制ws2812b实现呼吸灯等功能**


## 认识 [MimiClaw](https://github.com/memovai/mimiclaw)

- **小巧** — 没有 Linux，没有 Node.js，没有臃肿依赖 — 纯 C
- **好用** — 在 Telegram 发消息，剩下的它来搞定
- **忠诚** — 从记忆中学习，跨重启也不会忘
- **能干** — USB 供电，0.5W，24/7 运行
- **可爱** — 一块 ESP32-S3 开发板，$5，没了


## 相比于MimiClaw，esp32Claw做了以下修改：

- **使用MiniMax的API，每月29RMB**
- **增加了WS2812B驱动，实现呼吸灯功能**
- **增加了设备状态查询工具，可以通过飞书查询设备WiFi、IP、运行时长、内存、PSRAM、当前灯模式、颜色、亮度和呼吸周期等信息**
- **修改了WIFI重连逻辑，避免宕机**
- **优化了只配置了飞书的条件下，Telegram支线依然运行的问题**
- **优化了连接wifi后，本地管理AP常驻问题，节省资源**

## 快速开始

### 你需要

- 一块 **ESP32-S3 开发板**，16MB Flash + 8MB PSRAM
- 一根 **USB Type-C 数据线**
- 一个 **飞书机器人**
- 一个 **大模型API**

### Ubuntu和macOS 安装，参照[MimiClaw](https://github.com/memovai/mimiclaw)
### Windows11 安装

```bash
# 需要先安装 ESP-IDF v5.5+:
# 若git clone下载不下来，到官网找到5.5.2版本下载zip
git clone -b v5.5.2 --recursive https://github.com/espressif/esp-idf.git

git clone https://github.com/zhangjun2636808827/esp32claw


# 先安装idf
cd esp-idf-v5.5.2
# 终端进入cmd，不然安装后识别不到idf.py
cmd
# 安装
install.bat
export.bat 
# 验证安装
idf.py --help
# 得到以下结果表明在这个终端安装成功
```
<img src="assets/install.png" alt="" width="480" />

```bash
# 进入esp32claw目录
cd esp32claw
# 选择芯片
idf.py set-target esp32s3
# 复制配置文件，所有的信息都填写在这个文件中
cp main/mimi_secrets.h.example main/mimi_secrets.h
```
<img src="assets\config.png" alt="" width="480" />

### 获取[飞书](https://open.feishu.cn/app )信息
<img src="assets\feishu.png" alt="" width="480" />
<img src="assets\feishu2.png" alt="" width="480" />

### 复制 App ID 和 App Secret 到 MIMI_SECRET_FEISHU_APP_ID 和 MIMI_SECRET_FEISHU_APP_SECRET

<img src="assets\feishu3.png" alt="" width="480" />

### 创建机器人

<img src="assets\feishu4.png" alt="" width="480" />

### 配置机器人权限

<img src="assets\feishu5.png" alt="" width="480" />

```bash
# 粘贴以下josn数据

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
    "user": ["aily:file:read", "aily:file:write", "im:chat.access_event.bot_p2p_chat:read"]
  }
}
```

<img src="assets\feishu6.png" alt="" width="480" />

### 开启以下权限

<img src="assets\feishu7.png" alt="" width="480" />

### 配置事件订阅

<img src="assets\feishu8.png" alt="" width="480" />

### 发布应用

<img src="assets\feishu9.png" alt="" width="480" />
<img src="assets\feishu10.png" alt="" width="480" />

### 飞书创建一个群聊，然后点击设置-> 群机器人 -> 添加 -> 选择你前面创建的机器人 -> 在群里@你的机器人并发送消息，例如“@esp32claw 帮我打开蓝色呼吸灯，周期为2s”

<img src="assets\feishu11.png" alt="" width="480" />

### 到这里飞书就配置完了，接下来配置[MiniMax](https://www.minimaxi.com/)信息

### 订阅一个[MiniMax](https://www.minimaxi.com/)的29RMB的Starter套餐

### 账户管理 -> Token Plan -> 复制 Token Plan Key 到 MIMI_SECRET_API_KEY

<img src="assets\MiniMax1.png" alt="" width="480" />

```bash
# 填写以下信息
#define MIMI_SECRET_MODEL           "MiniMax-M2.7"//模型名称，详见https://minimaxi.com/docs/models
#define MIMI_SECRET_MODEL_PROVIDER  "openai"//提供者，默认为Anthropic，如果使用MiniMax系列模型请设置为"openai"
```

### 到这里就完成了mimi_secrets.h文件所有信息的填写，

### 配置WS2812B，打开mimiclaw-main\main\mimi_config.h文件

<img src="assets\image8.png" alt="" width="480" />

### 接下来编译并烧写代码

### esp32开发板连接电脑，注意要接上烧录口

<img src="assets\image.png" alt="" width="480" />

### 打开设备管理器

<img src="assets\image2.png" alt="" width="480" />

### 得到你的烧录口接到了哪个端口

<img src="assets\image3.png" alt="" width="480" />

### 可以看到我们的端口是COM5,接下来编译代码

```bash
# 完整编译（修改 mimi_secrets.h 后必须 fullclean）
idf.py fullclean && idf.py build
# 
idf.py -p COM5 flash monitor
```

<img src="assets\image4.png" alt="" width="480" />

<img src="assets\image5.png" alt="" width="480" />

### 出现以下表示编译和烧录成功

<img src="assets\image6.png" alt="" width="480" />

<img src="assets\image7.png" alt="" width="480" />


### 把usb线接到串口，打开MobaXterm(在MobaXterm/MobaXterm.exe)，按照以下步骤打开串口

<img src="assets\image9.png" alt="" width="480" />
<img src="assets\image10.png" alt="" width="480" />

### 按下esp32开发板上的复位按键

<img src="assets\image11.png" alt="" width="480" />


### 现在开始聊天！


<img src="assets\image12.jpg" alt="" width="480" />

<img src="assets\image13.jpg" alt="" width="480" />

<img src="assets\image14.png" alt="" width="480" />


### CLI 命令（通过 UART/COM 口连接）

通过串口连接即可配置和调试。**配置命令**让你无需重新编译就能修改设置 — 随时随地插上 USB 线就能改。

**运行时配置**（存入 NVS，覆盖编译时默认值）：

```
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

```
mimi> wifi_status              # 连上了吗？
mimi> memory_read              # 看看它记住了什么
mimi> memory_write "内容"       # 写入 MEMORY.md
mimi> heap_info                # 还剩多少内存？
mimi> session_list             # 列出所有会话
mimi> session_clear 12345      # 删除一个会话
mimi> heartbeat_trigger           # 手动触发一次心跳检查
mimi> cron_start                  # 立即启动 cron 调度器
mimi> restart                     # 重启
```

## 记忆

MimiClaw 把所有数据存为纯文本文件，可以直接读取和编辑：

| 文件 | 说明 |
|------|------|
| `SOUL.md` | 机器人的人设 — 编辑它来改变行为方式 |
| `USER.md` | 关于你的信息 — 姓名、偏好、语言 |
| `MEMORY.md` | 长期记忆 — 它应该一直记住的事 |
| `HEARTBEAT.md` | 待办清单 — 机器人定期检查并自主执行 |
| `cron.json` | 定时任务 — AI 创建的周期性或一次性任务 |
| `2026-02-05.md` | 每日笔记 — 今天发生了什么 |
| `tg_12345.jsonl` | 聊天记录 — 你和它的对话 |

## 工具

MimiClaw 同时支持 Anthropic 和 OpenAI 的工具调用 — LLM 在对话中可以调用工具，循环执行直到任务完成（ReAct 模式）。

| 工具 | 说明 |
|------|------|
| `web_search` | 通过 Tavily（优先）或 Brave 搜索网页，获取实时信息 |
| `get_current_time` | 通过 HTTP 获取当前日期和时间，并设置系统时钟 |
| `cron_add` | 创建定时或一次性任务（LLM 自主创建 cron 任务） |
| `cron_list` | 列出所有已调度的 cron 任务 |
| `cron_remove` | 按 ID 删除 cron 任务 |

启用网页搜索可在 `mimi_secrets.h` 中设置 [Tavily API key](https://app.tavily.com/home)（优先，`MIMI_SECRET_TAVILY_KEY`），或 [Brave Search API key](https://brave.com/search/api/)（`MIMI_SECRET_SEARCH_KEY`）。

## 定时任务（Cron）

MimiClaw 内置 cron 调度器，让 AI 可以自主安排任务。LLM 可以通过 `cron_add` 工具创建周期性任务（"每 N 秒"）或一次性任务（"在某个时间戳"）。任务触发时，消息会注入到 Agent 循环 — AI 自动醒来、处理任务并回复。

任务持久化存储在 SPIFFS（`cron.json`），重启后不会丢失。典型用途：每日总结、定时提醒、定期巡检。

## 心跳（Heartbeat）

心跳服务会定期读取 SPIFFS 上的 `HEARTBEAT.md`，检查是否有待办事项。如果发现未完成的条目（非空行、非标题、非已勾选的 `- [x]`），就会向 Agent 循环发送提示，让 AI 自主处理。

这让 MimiClaw 变成一个主动型助理 — 把任务写入 `HEARTBEAT.md`，机器人会在下一次心跳周期自动拾取执行（默认每 30 分钟）。

## 其他功能

- **WebSocket 网关** — 端口 18789，局域网内用任意 WebSocket 客户端连接
- **OTA 更新** — WiFi 远程刷固件，无需 USB
- **双核** — 网络 I/O 和 AI 处理分别跑在不同 CPU 核心
- **HTTP 代理** — CONNECT 隧道，适配受限网络
- **多提供商** — 同时支持 Anthropic (Claude) 和 OpenAI (GPT)，运行时可切换
- **定时任务** — AI 可自主创建周期性和一次性任务，重启后持久保存
- **心跳服务** — 定期检查任务文件，驱动 AI 自主执行
- **工具调用** — ReAct Agent 循环，两种提供商均支持工具调用


## 许可证

MIT

## 致谢

感谢 MimiClaw 的开发者。

点击进入[MimiClaw](https://github.com/memovai/mimiclaw)主页。

