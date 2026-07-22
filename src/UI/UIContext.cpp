#include "KargaTarayici/UI/UIContext.h"
#include <imgui.h>

namespace KargaTarayici::UI {

void UIContext::RenderAllPanels() {
    scannerWindow_.Render();
    logWindow_.Render();

    if (scannerWindow_.HasSelection()) {
        const auto& selected = scannerWindow_.GetSelectedItem();
        assemblyInspector_.SetTargetAddress(selected.address, selected.symbol, {});
    }

    assemblyInspector_.Render();
}

}
