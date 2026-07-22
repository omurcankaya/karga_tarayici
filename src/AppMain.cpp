#include "KargaTarayici/Core/IMemoryReader.h"
#include "KargaTarayici/Engine/ScanPipeline.h"
#include "KargaTarayici/Rules/JsonRuleParser.h"
#include <iostream>

int main(int argc, char* argv[]) {
    using namespace KargaTarayici;

    std::cout << "KargaScannerApp Started (Standalone Mode)\n";

    if (argc < 2) {
        std::cout << "Usage: KargaScannerApp.exe <rules.json>\n";
        return 1;
    }

    auto rules = Rules::JsonRuleParser::ParseFile(argv[1]);
    std::cout << "Loaded " << rules.size() << " scanner rules.\n";

    Engine::ScanPipeline pipeline;
    Core::DirectMemoryReader reader;

    auto results = pipeline.Run(0, reader, rules);
    std::cout << "Scan completed with " << results.size() << " results.\n";

    return 0;
}
