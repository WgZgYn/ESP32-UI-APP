#include "UI_VIEW.h"

#include <service/service.h>
#include "launcher.h"

astra::ui::LifecycleItem *rootPage = &ServiceUI::getInstance();
astra::Launcher astraLauncher = astra::Launcher(rootPage);

void UIConfig() {
}
