# 前言
其实是信息科学与实践大作业……

刚好撞上了之前的 idea：做一个桌面的待办事项小组件，注重 ui 和用户的动画体验。虽然本意是想探索 ui 设计工作流的，但是先写个模型层当然也不是不行（纯 ai 写丑的要死）

## MVC 第一阶段实现记录

第一阶段采用手写 MVC：

- Model 层：`TodoItem`、`LaunchAction`、`TodoRepository`、`TodoService`、`LaunchActionService`
- Controller 层：`TodoController`
- View 层：简陋 Qt Widgets，包括 `MainWindow`、`TodoListView`、`TodoItemWidget`、`TodoEditorDialog`

当前版本优先验证架构和功能闭环，后续再重写 View 层以实现更好的桌面组件体验和动画。

