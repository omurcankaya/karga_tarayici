import re
import json
import sys
import os

def parse_cpp_file(file_path):
    with open(file_path, 'r', encoding='utf-8', errors='ignore') as f:
        content = f.read()

    wrapper_pattern = re.compile(r'PyObject\s*\*\s*(\w+)\s*\([^)]*\)\s*\{([\s\S]*?)\n\}', re.MULTILINE)
    
    param_int_pattern = re.compile(r'PyTuple_GetInteger|PyTuple_GetUnsignedLong|PyTuple_GetLong|PyTuple_GetUnsignedInteger|PyTuple_GetByte')
    param_str_pattern = re.compile(r'PyTuple_GetString')
    param_float_pattern = re.compile(r'PyTuple_GetFloat')

    instance_ref_pattern = re.compile(r'(\w+)\s*[\*&]\s*(\w+)\s*=\s*(\w+)::(?:Instance|GetSingleton|InstancePtr)\(\);')
    direct_call_pattern = re.compile(r'(\w+)::(?:Instance|GetSingleton|InstancePtr)\(\)\s*[\.\->]\s*(\w+)\s*\(([^)]*)\)')

    rules = []

    for match in wrapper_pattern.finditer(content):
        wrapper_name = match.group(1)
        body = match.group(2)

        param_types = []
        if param_int_pattern.search(body):
            param_types.append("int")
        if param_str_pattern.search(body):
            param_types.append("string")
        if param_float_pattern.search(body):
            param_types.append("float")

        var_to_class = {}
        for ref_match in instance_ref_pattern.finditer(body):
            cls_name = ref_match.group(1)
            var_name = ref_match.group(2)
            var_to_class[var_name] = cls_name

        calls = []

        for direct_match in direct_call_pattern.finditer(body):
            cls_name = direct_match.group(1)
            method_name = direct_match.group(2)
            args_str = direct_match.group(3).strip()
            arg_count = len([a for a in args_str.split(',') if a.strip()]) if args_str else 0

            calls.append({
                "class_name": cls_name,
                "method_name": method_name,
                "arg_count": arg_count
            })

        for var_name, cls_name in var_to_class.items():
            var_call_pattern = re.compile(r'\b' + re.escape(var_name) + r'[\.\->]\s*(\w+)\s*\(([^)]*)\)')
            for var_match in var_call_pattern.finditer(body):
                method_name = var_match.group(1)
                args_str = var_match.group(2).strip()
                arg_count = len([a for a in args_str.split(',') if a.strip()]) if args_str else 0

                calls.append({
                    "class_name": cls_name,
                    "method_name": method_name,
                    "arg_count": arg_count
                })

        if calls:
            rule = {
                "wrapper_name": wrapper_name,
                "param_signature": param_types,
                "targets": calls
            }
            rules.append(rule)

    return rules

def main():
    target_path = r"C:\Users\omurcan\Desktop\extraction_metin2"
    if len(sys.argv) >= 2:
        target_path = sys.argv[1]

    all_rules = []

    if os.path.isfile(target_path):
        all_rules.extend(parse_cpp_file(target_path))
    elif os.path.isdir(target_path):
        for root, _, files in os.walk(target_path):
            for file in sorted(files):
                if file.endswith('.cpp') or file.endswith('.h'):
                    file_rules = parse_cpp_file(os.path.join(root, file))
                    all_rules.extend(file_rules)
                    print(f"Parsed {file}: {len(file_rules)} wrapper rules.")

    output_dir = "config"
    if not os.path.exists(output_dir):
        os.makedirs(output_dir)

    output_file = os.path.join(output_dir, "rules_auto_generated.json")
    with open(output_file, 'w', encoding='utf-8') as f:
        json.dump(all_rules, f, indent=2)

    print(f"\n[SUCCESS] Total Parsed Rules: {len(all_rules)}")
    print(f"[SUCCESS] Generated: {os.path.abspath(output_file)}")

if __name__ == "__main__":
    main()
