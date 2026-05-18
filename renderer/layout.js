function animateListShift(previousRects, options = {}) {
  if (options.onlyMovedDown) {
    getAnimatedItems().forEach((item) => {
      const previous = previousRects.get(item);
      if (!previous) return;
      const current = item.getBoundingClientRect();
      const deltaY = previous.top - current.top;
      if (deltaY <= 0) return;
      item.animate(
        [{ transform: `translateY(${deltaY}px)` }, { transform: "translateY(0)" }],
        { duration: LIST_SHIFT_ANIMATION_MS, easing: "cubic-bezier(.2, .8, .2, 1)" }
      );
    });
    return;
  }

  getAnimatedItems().forEach((item) => {
    const previous = previousRects.get(item);
    if (!previous) return;
    const current = item.getBoundingClientRect();
    const deltaY = previous.top - current.top;
    if (!deltaY) return;
    item.animate(
      [{ transform: `translateY(${deltaY}px)` }, { transform: "translateY(0)" }],
      { duration: LIST_SHIFT_ANIMATION_MS, easing: "cubic-bezier(.2, .8, .2, 1)" }
    );
  });
}

function getWidgetMaxHeight() {
  return Math.min(615, Math.max(240, (window.screen?.availHeight || 695) - 80));
}

function measureWidgetContentHeight() {
  const children = Array.from(taskList.children).filter((child) => {
    return child.offsetParent !== null &&
      !child.classList.contains("drag-ghost") &&
      !child.classList.contains("is-removing");
  });

  if (!children.length) return 0;

  const styles = getComputedStyle(taskList);
  const bottomPadding = parseFloat(styles.paddingBottom) || 0;
  const layoutBottom = children.reduce((bottom, child) => {
    return Math.max(bottom, child.offsetTop + child.offsetHeight);
  }, 0);

  return Math.ceil(layoutBottom + bottomPadding);
}

function applyWidgetHeight() {
  const maxHeight = getWidgetMaxHeight();
  const contentHeight = measureWidgetContentHeight();
  const shouldScroll = contentHeight > maxHeight + 1;
  const nextHeight = Math.min(contentHeight, maxHeight);

  todoWidget.classList.toggle("is-scrollable", shouldScroll);
  todoWidget.style.height = shouldScroll ? `${nextHeight}px` : "auto";
  syncDesktopWindowHeight(nextHeight);
}

function initializeWidgetHeight() {
  heightTransitionsEnabled = false;
  todoWidget.classList.remove("is-height-ready");
  applyWidgetHeight();
}

function enableWidgetHeightTransitions() {
  applyWidgetHeight();
  requestAnimationFrame(() => {
    heightTransitionsEnabled = true;
    todoWidget.classList.add("is-height-ready");
  });
}

function settleWidgetHeight() {
  requestAnimationFrame(() => {
    if (heightTransitionsEnabled) animateWidgetHeight();
    else applyWidgetHeight();
  });
}

function snapshotWidgetHeight() {
  return todoWidget.getBoundingClientRect().height || Math.min(measureWidgetContentHeight(), getWidgetMaxHeight());
}

function animateWidgetHeight(previousHeight = snapshotWidgetHeight()) {
  applyWidgetHeight();
}

function animateWidgetHeightAfterListShift(previousHeight, holdId = 0) {
  const heldHeight = Math.ceil(previousHeight || snapshotWidgetHeight());
  todoWidget.style.height = `${heldHeight}px`;
  syncDesktopWindowHeight(heldHeight, { force: true });

  window.setTimeout(() => {
    releaseDesktopWindowHeight(holdId);
    animateWidgetHeight(previousHeight);
  }, LIST_SHIFT_ANIMATION_MS + 40);
}

function settleWidgetHeightAfterPanelTransition(task, holdId) {
  const panel = task?.querySelector(".subtask-panel");
  const settle = () => {
    releaseDesktopWindowHeight(holdId);
    settleWidgetHeight();
  };
  window.clearTimeout(panelSettleTimer);
  if (panel) {
    panel.addEventListener("transitionend", settle, { once: true });
  }
  panelSettleTimer = window.setTimeout(settle, 360);
}

function holdDesktopWindowHeight(contentHeight) {
  desktopResizeHoldId += 1;
  desktopResizeHeld = true;
  syncDesktopWindowHeight(contentHeight, { force: true });
  return desktopResizeHoldId;
}

function releaseDesktopWindowHeight(holdId = desktopResizeHoldId) {
  if (holdId !== desktopResizeHoldId) return;
  desktopResizeHeld = false;
}

function syncDesktopWindowHeight(contentHeight = todoWidget.getBoundingClientRect().height, options = {}) {
  if (!window.floatingTodoDesktop?.resizeToContent) return;
  if (desktopResizeHeld && !options.force) return;
  if (desktopResizeFrame) cancelAnimationFrame(desktopResizeFrame);
  desktopResizeFrame = requestAnimationFrame(() => {
    desktopResizeFrame = 0;
    const bodyStyles = getComputedStyle(document.body);
    const verticalPadding =
      (parseFloat(bodyStyles.paddingTop) || 0) +
      (parseFloat(bodyStyles.paddingBottom) || 0);
    const height = Math.ceil(contentHeight + verticalPadding);
    window.floatingTodoDesktop.resizeToContent(height);
  });
}

function snapshotMovables() {
  return new Map(
    getAnimatedItems()
      .filter((item) => !item.classList.contains("is-dragging"))
      .map((item) => [item, item.getBoundingClientRect()])
  );
}
