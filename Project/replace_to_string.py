import os
import re

directories = [
    r"C:\Users\k023g\Desktop\DirectXGame\Project\Engine",
    r"C:\Users\k023g\Desktop\DirectXGame\Project\Game"
]

def add_format_header(content):
    if '#include <format>' not in content:
        if '#pragma once' in content:
            content = content.replace('#pragma once', '#pragma once\n#include <format>', 1)
        else:
            match = re.search(r'#include\s*[<"][^>"]+[>"]', content)
            if match:
                content = content[:match.start()] + '#include <format>\n' + content[match.start():]
            else:
                content = '#include <format>\n' + content
    return content

def process_file(filepath):
    with open(filepath, 'r', encoding='utf-8') as f:
        content = f.read()

    original_content = content

    # Pre-replacements for very specific cases
    content = content.replace(
        'categories_[i].name + " (" + std::to_string(categories_[i].effects.size()) + " effects)"',
        'std::format("{} ({} effects)", categories_[i].name, categories_[i].effects.size())'
    )
    content = content.replace(
        '"KeyFrame " + std::to_string(i) + " (t:" + std::to_string(sectionElements_[i]->GetTimePoint()) + ")"',
        'std::format("KeyFrame {} (t:{})", i, sectionElements_[i]->GetTimePoint())'
    )
    content = content.replace(
        'groupName_ +std::to_string(number).c_str()',
        'std::format("{}{}", groupName_, number)'
    )
    content = content.replace(
        '"Gamepad " + std::to_string(i) + " initialized successfully\\n"',
        'std::format("Gamepad {} initialized successfully\\n", i)'
    )
    content = content.replace(
        '"Gamepad " + std::to_string(i) + " initialization failed\\n"',
        'std::format("Gamepad {} initialization failed\\n", i)'
    )
    content = content.replace(
        '"Number of gamepads initialized: " + std::to_string(gamepads_.size()) + "\\n"',
        'std::format("Number of gamepads initialized: {}\\n", gamepads_.size())'
    )
    content = content.replace(
        '"KeyFrameContextMenu_" + std::to_string(trackIndex) + "_" + std::to_string(keyIndex)',
        'std::format("KeyFrameContextMenu_{}_{}", trackIndex, keyIndex)'
    )
    content = content.replace(
        'groupName_ += std::to_string(audienceIndex_);',
        'groupName_ += std::format("{}", audienceIndex_);'
    )
    content = content.replace(
        'std::to_string(trackIndex) + "_" + std::to_string(keyIndex)',
        'std::format("{}_{}", trackIndex, keyIndex)'
    )

    # 1. "String" + std::to_string(var) + "String"
    content = re.sub(
        r'"([^"]*)"\s*\+\s*std::to_string\(([^()]*|\([^()]*\))\)\s*\+\s*"([^"]*)"',
        r'std::format("\1{}\3", \2)',
        content
    )

    # 2. var + "String" + std::to_string(var)
    content = re.sub(
        r'([a-zA-Z_]\w*)\s*\+\s*"([^"]*)"\s*\+\s*std::to_string\(([^()]*|\([^()]*\))\)',
        r'std::format("{}\2{}", \1, \3)',
        content
    )

    # 3. ("String" + std::to_string(var)).c_str()
    content = re.sub(
        r'\(\s*"([^"]*)"\s*\+\s*std::to_string\(([^()]*|\([^()]*\))\)\s*\)\.c_str\(\)',
        r'std::format("\1{}", \2).c_str()',
        content
    )

    # 3b. ("String" + std::to_string(var) + "String").c_str()
    # E.g. if previous regex didn't catch it

    # 4. "String" + std::to_string(var)
    content = re.sub(
        r'"([^"]*)"\s*\+\s*std::to_string\(([^()]*|\([^()]*\))\)',
        r'std::format("\1{}", \2)',
        content
    )

    # 5. std::to_string(var) + "String"
    content = re.sub(
        r'std::to_string\(([^()]*|\([^()]*\))\)\s*\+\s*"([^"]*)"',
        r'std::format("{}\2", \1)',
        content
    )

    # 6. var + std::to_string(var)
    content = re.sub(
        r'([a-zA-Z_]\w*)\s*\+\s*std::to_string\(([^()]*|\([^()]*\))\)',
        r'std::format("{}{}", \1, \2)',
        content
    )

    # 7. += std::to_string(var)
    content = re.sub(
        r'\+=\s*std::to_string\(([^()]*|\([^()]*\))\)',
        r'+= std::format("{}", \1)',
        content
    )

    # After these, do we have any (std::format(...)).c_str()? 
    # e.g., ("Section " + std::to_string(i)).c_str()
    # It would be (std::format("Section {}", i)).c_str()
    # Let's fix that so it removes the outer parens:
    content = re.sub(
        r'\(\s*(std::format\([^)]+\))\s*\)\.c_str\(\)',
        r'\1.c_str()',
        content
    )

    if content != original_content:
        content = add_format_header(content)
        with open(filepath, 'w', encoding='utf-8') as f:
            f.write(content)
        print(f"Updated {filepath}")

if __name__ == '__main__':
    for d in directories:
        for root, _, files in os.walk(d):
            for file in files:
                if file.endswith('.cpp') or file.endswith('.h'):
                    process_file(os.path.join(root, file))
