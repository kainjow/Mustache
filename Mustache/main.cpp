//
//  main.cpp
//  Mustache
//
//  Created by Kevin Wojniak on 4/11/15.
//  Copyright (c) 2015 Kevin Wojniak. All rights reserved.
//

#include <iostream>
#include <sstream>
#include <vector>
#include <functional>

namespace Mustache {

template <typename StringType>
StringType trim(const StringType &s) {
    auto it = s.begin();
    while (it != s.end() && isspace(static_cast<int>(*it))) {
        it++;
    }
    auto rit = s.rbegin();
    while (rit.base() != it && isspace(*rit)) {
        rit++;
    }
    return StringType(it, rit.base());
}

template <typename StringType>
class Context {
public:
    bool getVar(const StringType& name, StringType& result) {
        if (name == "name") {
            result = "Kevin";
            return true;
        } else if (name == "ender") {
            result = "";
            return true;
        } else if (name == "dayOfWeek") {
            result = "Monday";
            return true;
        } else if (name == "alive") {
            result = "uh";
            return true;
        } else {
            std::cout << "CONTEXT MISSING " << name << std::endl;
        }
        return false;
    }
};

template <typename StringType>
class Mustache {
public:
    Mustache(const StringType& input) {
        parse(input);
    }
    
    bool isValid() const {
        return errorMessage_.empty();
    }

    const StringType& errorMessage() const {
        return errorMessage_;
    }
    
    template <typename OStream>
    void render(OStream& stream, Context<StringType>& ctx) {
        walk([&stream, &ctx](Component& comp, int) -> WalkControl {
            if (comp.isText()) {
                stream << comp.text;
            } else if (comp.isTag()) {
                const Tag& tag(comp.tag);
                StringType var;
                switch (tag.type) {
                    case Tag::Type::Variable:
                        if (ctx.getVar(tag.name, var)) {
                            // TODO: escape
                            stream << var;
                        }
                        break;
                    case Tag::Type::UnescapedVariable:
                        if (ctx.getVar(tag.name, var)) {
                            stream << var;
                        }
                        break;
                    case Tag::Type::SectionBegin:
                        if (ctx.getVar(tag.name, var)) {
                            
                        } else {
                            return WalkControl::Skip;
                        }
                        break;
                    case Tag::Type::SectionBeginInverted:
                        std::cout << "RENDER INVSECTION: " << tag.name << std::endl;
                        break;
                    case Tag::Type::Partial:
                        std::cout << "RENDER PARTIAL: " << tag.name << std::endl;
                        break;
                    case Tag::Type::SetDelimiter:
                        std::cout << "RENDER SETDELIM: " << tag.name << std::endl;
                        break;
                    default:
                        break;
                }
            }
            return WalkControl::Continue;
        });
    }
    
private:
    using StringSizeType = typename StringType::size_type;

    class Tag {
    public:
        enum class Type {
            Invalid,
            Variable,
            UnescapedVariable,
            SectionBegin,
            SectionEnd,
            SectionBeginInverted,
            Comment,
            Partial,
            SetDelimiter,
        };
        StringType name;
        Type type = Type::Invalid;
        bool isSectionBegin() const {
            return type == Type::SectionBegin || type == Type::SectionBeginInverted;
        }
        bool isSectionEnd() const {
            return type == Type::SectionEnd;
        }
    };
    
    class Component {
    public:
        StringType text;
        Tag tag;
        std::vector<Component> children;
        StringSizeType position = StringType::npos;
        bool isText() const {
            return !text.empty();
        }
        bool isTag() const {
            return text.empty();
        }
    };
    
    void parse(const StringType& input) {
        using streamstring = std::basic_ostringstream<typename StringType::value_type>;
        
        const StringType braceDelimiterBegin(2, '{');
        const StringType braceDelimiterEnd(2, '}');
        const StringType braceDelimiterEndUnescaped(3, '}');
        const StringSizeType inputSize = input.size();
        
        StringType currentDelimiterBegin(braceDelimiterBegin);
        StringType currentDelimiterEnd(braceDelimiterEnd);
        bool currentDelimiterIsBrace = true;
        
        std::vector<Component*> sections;
        sections.push_back(&rootComponent_);
        
        StringSizeType inputPosition = 0;
        while (inputPosition != inputSize) {
            
            // Find the next tag start delimiter
            const StringSizeType tagLocationStart = input.find(currentDelimiterBegin, inputPosition);
            if (tagLocationStart == StringType::npos) {
                Component comp;
                comp.text = StringType(input, inputPosition, inputSize - inputPosition);
                comp.position = inputPosition;
                sections.back()->children.push_back(comp);
                break;
            } else if (tagLocationStart != inputPosition) {
                Component comp;
                comp.text = StringType(input, inputPosition, tagLocationStart - inputPosition);
                comp.position = inputPosition;
                sections.back()->children.push_back(comp);
            }
            
            StringSizeType tagContentsLocation = tagLocationStart + currentDelimiterBegin.size();
            
            // Find the next tag end delimiter
            const bool tagIsUnescapedVar = currentDelimiterIsBrace && tagLocationStart != (inputSize - 2) && input.at(tagContentsLocation) == braceDelimiterBegin.at(0);
            const StringType& currenttTagDelimiterEnd(tagIsUnescapedVar ? braceDelimiterEndUnescaped : currentDelimiterEnd);
            if (tagIsUnescapedVar) {
                ++tagContentsLocation;
            }
            StringSizeType tagLocationEnd = input.find(currenttTagDelimiterEnd, tagContentsLocation);
            if (tagLocationEnd == StringType::npos) {
                streamstring ss;
                ss << "No tag end delimiter found for start delimiter at " << tagLocationStart;
                errorMessage_.assign(ss.str());
                return;
            }
            
            // Create a Tag object representation
            StringType tagContents(trim(StringType(input, tagContentsLocation, tagLocationEnd - tagContentsLocation)));
            Component comp;
            parseTagContents(tagIsUnescapedVar, tagContents, comp.tag);
            comp.position = tagLocationStart;
            sections.back()->children.push_back(comp);
            
            if (comp.tag.isSectionBegin()) {
                sections.push_back(&sections.back()->children.back());
            } else if (comp.tag.isSectionEnd()) {
                if (sections.size() == 1) {
                    streamstring ss;
                    ss << "Section end tag \"" << comp.tag.name << "\" found without start tag at " << comp.position;
                    errorMessage_.assign(ss.str());
                    return;
                }
                sections.pop_back();
            }
            
            inputPosition = tagLocationEnd + currenttTagDelimiterEnd.size();
        }
        
#if 0
        walk([](Component& comp, int depth) -> bool {
            const StringType indent = depth >= 1 ? StringType(depth, ' ') : StringType();
            if (comp.isTag()) {
                std::cout << indent << "TAG: {{" << comp.tag.name << "}}" << std::endl;
            } else {
                std::cout << indent << "TXT: " << comp.text << std::endl;
            }
            return true;
        });
#endif

        // Check for sections without an ending tag
        const Component *invalidStartPosition = nullptr;
        walk([&invalidStartPosition](Component& comp, int) -> WalkControl {
            if (!comp.tag.isSectionBegin()) {
                return WalkControl::Continue;
            }
            if (comp.children.empty() || !comp.children.back().tag.isSectionEnd() || comp.children.back().tag.name != comp.tag.name) {
                invalidStartPosition = &comp;
                return WalkControl::Stop;
            }
            comp.children.pop_back(); // remove now useless end section component
            return WalkControl::Continue;
        });
        if (invalidStartPosition) {
            streamstring ss;
            ss << "No section end tag found for section \"" << invalidStartPosition->tag.name << "\" at " << invalidStartPosition->position;
            errorMessage_.assign(ss.str());
            return;
        }
    }
    
    enum class WalkControl {
        Continue,
        Stop,
        Skip,
    };
    using WalkCallback = std::function<WalkControl(Component&, int)>;
    void walk(const WalkCallback& callback) const {
        for (auto comp : rootComponent_.children) {
            const WalkControl control = walk(callback, comp, 0);
            if (control != WalkControl::Continue) {
                break;
            }
        }
    }
    
    WalkControl walk(const WalkCallback& callback, Component& comp, int depth) const {
        WalkControl control = callback(comp, depth);
        if (control == WalkControl::Stop) {
            return control;
        } else if (control == WalkControl::Skip) {
            return WalkControl::Continue;
        }
        ++depth;
        for (auto childComp : comp.children) {
            control = walk(callback, childComp, depth);
            if (control == WalkControl::Stop) {
                return control;
            } else if (control == WalkControl::Skip) {
                control = WalkControl::Continue;
                break;
            }
        }
        --depth;
        return control;
    }
    
    void parseTagContents(bool isUnescapedVar, const StringType& contents, Tag& tag) {
        if (isUnescapedVar) {
            tag.type = Tag::Type::UnescapedVariable;
            tag.name = contents;
        } else if (contents.empty()) {
            tag.type = Tag::Type::Variable;
            tag.name.clear();
        } else {
            switch (static_cast<char>(contents.at(0))) {
                case '#':
                    tag.type = Tag::Type::SectionBegin;
                    break;
                case '^':
                    tag.type = Tag::Type::SectionBeginInverted;
                    break;
                case '/':
                    tag.type = Tag::Type::SectionEnd;
                    break;
                case '>':
                    tag.type = Tag::Type::Partial;
                    break;
                case '&':
                    tag.type = Tag::Type::UnescapedVariable;
                    break;
                case '!':
                    tag.type = Tag::Type::Comment;
                    break;
                default:
                    tag.type = Tag::Type::Variable;
                    break;
            }
            if (tag.type == Tag::Type::Variable) {
                tag.name = contents;
            } else {
                StringType name(contents);
                name.erase(name.begin());
                tag.name = trim(name);
            }
        }
    }
    
private:
    StringType errorMessage_;
    Component rootComponent_;
};

} // namespace

int main() {
    
    //Mustache::Mustache<std::wstring> templw(L"");
    
    std::string input = "Hello {{name}}! Today is {{dayOfWeek}}.{{#alive}}You're alive!.{{#inner}}inner stuff{{#deeper}}we must go deeper{{/deeper}}{{/inner}}{{/alive}}fin{{ender}}hi";
    Mustache::Mustache<std::string> templ(input);
    if (!templ.isValid()) {
        std::cout << "ERROR: " << templ.errorMessage() << std::endl;
        return 0;
    }
    
    Mustache::Context<std::string> ctx;
    
    std::stringstream ss;
    templ.render(ss, ctx);
    std::cout << ss.str() << std::endl;
    
    return 0;
}
