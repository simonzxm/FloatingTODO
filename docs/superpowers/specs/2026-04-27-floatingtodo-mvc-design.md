# FloatingTODO MVC 架构设计

## 1. 项目目标

FloatingTODO 是一个基于 Qt 的桌面待办事项组件。第一阶段目标不是追求复杂 UI 和动画，而是先完成一个清晰、可读、可扩展的 MVC 架构：

- 模型层负责表达任务、持久化数据和业务规则。
- 控制层负责接收视图层的用户意图，并协调模型层完成操作。
- 视图层先使用简陋 Qt Widgets 实现，验证完整功能流。
- 后续可以重写视图层，替换为更重视用户体验、动画和桌面组件感的 UI。

第一阶段架构要尽量简单，方便阅读和讲解，避免过度工程化。

## 2. 架构选择

项目采用“手写 MVC + Service + Repository”的架构。

Service 在本项目中归入广义 Model 层，负责业务规则；Repository 也归入 Model 层，负责 SQLite 持久化。Controller 不直接写业务细节，也不直接操作数据库；View 不直接访问数据库，只通过 Controller 发出用户操作。

```text
View
  ↓ 用户操作
Controller
  ↓ 协调业务
Service
  ↓ 读写数据
Repository
  ↓
SQLite
```

分层职责：

```text
Model 层
- TodoItem：任务实体
- LaunchAction：启动动作实体
- TodoRepository：SQLite 读写
- TodoService：任务业务规则
- LaunchActionService：打开网页、文件、文件夹、应用程序

Controller 层
- TodoController：View 的唯一业务入口

View 层
- MainWindow：主窗口
- TodoListView：任务列表
- TodoItemWidget：单个任务项
- TodoEditorDialog：新增/编辑任务弹窗
```

## 3. 推荐文件结构

```text
src/
  main.cpp

  model/
    TodoItem.h
    TodoItem.cpp
    LaunchAction.h
    LaunchAction.cpp

  repository/
    TodoRepository.h
    TodoRepository.cpp

  service/
    TodoService.h
    TodoService.cpp
    LaunchActionService.h
    LaunchActionService.cpp

  controller/
    TodoController.h
    TodoController.cpp

  view/
    MainWindow.h
    MainWindow.cpp
    TodoListView.h
    TodoListView.cpp
    TodoItemWidget.h
    TodoItemWidget.cpp
    TodoEditorDialog.h
    TodoEditorDialog.cpp
```

## 4. 任务模型

第一版砍掉任务自身进度条，只保留完成/未完成状态。

```text
TodoItem
- id
- parentId
- title
- dueAt
- completed
- launchAction
- createdAt
- updatedAt
```

字段说明：

- `id`：任务唯一标识。
- `parentId`：父任务 id。为空表示顶层任务。
- `title`：任务标题，必填。
- `dueAt`：截止时间，可为空。
- `completed`：任务自身是否完成。
- `launchAction`：可选启动动作。
- `createdAt`：创建时间。
- `updatedAt`：更新时间。

任务支持任意多级嵌套。

## 5. 启动动作模型

每个任务可以绑定一个启动动作，用来帮助用户从待办事项直接进入工作现场。

```text
LaunchAction
- type
- target
- displayName
```

启动动作类型：

```text
None
Url
File
Folder
Application
```

字段说明：

- `type`：启动动作类型。
- `target`：URL、本地文件路径、文件夹路径或应用程序路径。
- `displayName`：启动按钮显示名，可为空。为空时 View 可以显示默认文字“启动”。

执行规则：

- `Url`、`File`、`Folder` 使用 `QDesktopServices::openUrl()`。
- `Application` 使用 `QProcess::startDetached()`。
- 执行失败时返回错误信息，由 Controller 通知 View 显示。

## 6. SQLite 数据库设计

第一版使用一张 `todos` 表保存任务。

```text
todos
- id INTEGER PRIMARY KEY
- parent_id INTEGER NULL
- title TEXT NOT NULL
- due_at TEXT NULL
- completed INTEGER NOT NULL DEFAULT 0
- launch_type TEXT NOT NULL DEFAULT 'none'
- launch_target TEXT
- launch_display_name TEXT
- created_at TEXT NOT NULL
- updated_at TEXT NOT NULL
```

说明：

- `parent_id` 表示父子任务关系。
- 顶层任务的 `parent_id` 为空。
- 子任务统计不单独存表字段，而是在需要展示时动态计算。
- 任务自身没有进度字段，避免模型复杂化。

## 7. 核心业务规则

1. 任务只有完成和未完成两种状态。
2. 任务支持任意层级嵌套。
3. 第一版 UI 按缩进递归展示所有层级。
4. 当任务没有子任务时，只显示任务自身完成状态。
5. 当任务有直接子任务时，额外显示“已完成直接子任务数 / 直接子任务总数”。
6. 父任务是否完成由用户主动点击决定，不由子任务自动完成。
7. 删除父任务时，递归删除其所有子任务。
8. View 不直接访问 SQLite，只通过 Controller 操作。

子任务统计示例：

```text
□ 信息科学大作业    子任务：2/3
  □ 搭 MVC 架构      子任务：1/2
    ✓ 设计模型层
    □ 设计控制层
  ✓ 写项目笔记
  □ 做 UI 重写
```

这里的 `2/3` 只统计“信息科学大作业”的直接子任务，不统计更深层后代任务。

## 8. Repository 设计

`TodoRepository` 只负责数据库读写，不处理业务规则，不打开文件或网页。

建议接口：

```text
TodoRepository
- initialize()
- findAll()
- findById(id)
- add(item)
- update(item)
- remove(id)
- childrenOf(parentId)
- removeSubtree(id)
```

职责边界：

- 负责创建 SQLite 表。
- 负责插入、查询、更新、删除任务。
- 负责根据 `parent_id` 查询子任务。
- 可以提供递归删除接口，但不决定什么时候删除。

## 9. Service 设计

`TodoService` 负责待办任务的业务规则。

建议接口：

```text
TodoService
- loadTaskTree()
- createTask(parentId, title, dueAt, launchAction)
- updateTask(id, title, dueAt, launchAction)
- deleteTask(id)
- toggleCompleted(id)
- childStats(id)
```

职责：

- 创建任务时填充创建时间和更新时间。
- 修改任务时更新 `updatedAt`。
- 删除任务时要求 Repository 递归删除子任务。
- 切换完成状态时只影响当前任务。
- 计算直接子任务完成数量和总数量。
- 组装 View 需要的任务树数据。

`LaunchActionService` 负责启动动作。

建议接口：

```text
LaunchActionService
- launch(action) -> LaunchResult
```

`LaunchResult` 应表达：

```text
- success
- message
```

## 10. Controller 设计

`TodoController` 是 View 的唯一业务入口。View 不应该直接调用 Repository 或 Service。

建议接口：

```text
TodoController
- refresh()
- addRootTask(...)
- addChildTask(parentId, ...)
- editTask(id, ...)
- deleteTask(id)
- toggleTaskCompleted(id)
- launchTask(id)
```

建议信号：

```text
tasksChanged(taskTree)
errorOccurred(message)
infoOccurred(message)
```

Controller 职责：

- 接收 View 的用户意图。
- 调用 TodoService 或 LaunchActionService。
- 操作完成后刷新任务树。
- 将错误或提示信息转成信号通知 View。

## 11. 第一版 View 设计

第一版视图使用 Qt Widgets 手写，追求简单可用，不追求美观和动画。

```text
MainWindow
- 持有 TodoController
- 放置 TodoListView
- 顶部提供“新增顶层任务”按钮
- 显示错误和提示信息

TodoListView
- 接收任务树数据
- 清空并递归重建任务列表
- 按缩进展示所有层级任务
- 第一版不做折叠、拖拽、筛选

TodoItemWidget
- 显示标题
- 显示截止时间
- 显示完成状态
- 有子任务时显示“已完成直接子任务数 / 直接子任务总数”
- 提供完成/取消完成、启动、新增子任务、编辑、删除按钮

TodoEditorDialog
- 新建和编辑任务共用
- 输入标题
- 选择截止时间
- 选择启动动作类型
- 输入启动目标
- 输入启动按钮显示名
```

示意：

```text
[新增顶层任务]

□ 信息科学大作业        截止：2026-05-20    子任务：2/3
  [完成] [启动] [新增子任务] [编辑] [删除]

  □ 搭 MVC 架构          截止：2026-04-28    子任务：1/2
    [完成] [启动] [新增子任务] [编辑] [删除]

    ✓ 设计模型层          截止：2026-04-27
      [取消完成] [启动] [新增子任务] [编辑] [删除]

  ✓ 写项目笔记            截止：2026-04-29
    [取消完成] [启动] [新增子任务] [编辑] [删除]
```

## 12. 主要交互流

新增任务：

```text
用户点击新增
  ↓
View 打开 TodoEditorDialog
  ↓
View 调 TodoController::addRootTask 或 addChildTask
  ↓
Controller 调 TodoService::createTask
  ↓
Service 调 Repository 写入 SQLite
  ↓
Controller refresh
  ↓
View 收到 tasksChanged 后重画列表
```

切换完成状态：

```text
用户点击完成/取消完成
  ↓
View 调 TodoController::toggleTaskCompleted
  ↓
Controller 调 TodoService::toggleCompleted
  ↓
Service 更新当前任务 completed
  ↓
Controller refresh
  ↓
View 重新显示完成状态和子任务统计
```

启动任务：

```text
用户点击启动
  ↓
View 调 TodoController::launchTask
  ↓
Controller 查询任务
  ↓
Controller 调 LaunchActionService::launch
  ↓
成功则发 infoOccurred
失败则发 errorOccurred
```

删除任务：

```text
用户点击删除
  ↓
View 弹出确认框
  ↓
View 调 TodoController::deleteTask
  ↓
Controller 调 TodoService::deleteTask
  ↓
Service 要求 Repository 递归删除任务树
  ↓
Controller refresh
  ↓
View 重画列表
```

## 13. 错误处理

建议错误处理规则：

- 标题为空：Dialog 或 Controller 拦截，提示用户。
- 数据库初始化失败：Controller 发 `errorOccurred`。
- 数据库写入失败：Controller 发 `errorOccurred`。
- 启动动作类型为 `None`：可以提示“该任务没有设置启动动作”。
- 启动目标为空：返回失败信息。
- URL 或路径无法打开：返回失败信息。
- 删除任务前：View 弹出确认框。

## 14. 测试重点

第一阶段可以重点测试模型层和业务规则。

`TodoRepository`：

- 初始化数据库。
- 插入任务。
- 查询全部任务。
- 查询指定任务。
- 更新任务。
- 删除任务。
- 根据 `parent_id` 查询子任务。

`TodoService`：

- 新建根任务。
- 新建子任务。
- 切换完成状态。
- 删除父任务时递归删除子任务。
- 统计直接子任务完成数量。
- 组装任意层级任务树。

`LaunchActionService`：

- `None` 类型返回无启动动作。
- 空 target 返回失败。
- Url/File/Folder/Application 类型能进入正确分支。

View 层第一版主要通过手动运行验证：

- 能新增、编辑、删除任务。
- 能添加任意层级子任务。
- 能切换完成状态。
- 有子任务时能显示完成数量/总数量。
- 启动按钮能打开对应目标或显示失败信息。

## 15. 后续 UI 重写边界

第一阶段的朴素 Qt Widgets View 只是验证 MVC 架构和功能闭环。后续重写 UI 时应尽量保留：

```text
model/
repository/
service/
controller/
```

主要替换：

```text
view/
```

可替换方向：

- 更精致的 Qt Widgets 卡片式 UI。
- Qt Quick/QML 动画 UI。
- 桌面悬浮小组件。
- 自绘任务卡片和动效。

只要新 View 继续通过 `TodoController` 操作业务，就不需要重写模型层、数据库层和控制层。

## 16. 第一阶段不做的内容

为了保证架构简单、可读、可完成，第一阶段暂不实现：

- 任务拖拽排序。
- 标签、搜索、筛选。
- 提醒和通知。
- 重复任务。
- 复杂动画。
- 复杂主题系统。
- 云同步。
- 用户账户。

这些功能可以作为后续扩展，但不进入第一阶段 MVC 骨架。
