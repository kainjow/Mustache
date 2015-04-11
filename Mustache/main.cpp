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
    virtual StringType variable(const StringType& name);
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
    
private:
    class Tag {
    public:
        std::string name;
        enum class Type {
            Variable,
            UnescapedVariable,
            SectionBegin,
            SectionEnd,
            SectionBeginInverted,
            Comment,
            Partial,
            SetDelimiter,
        };
        Type type = Type::Variable;
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
        bool isText() const {
            return !text.empty();
        }
        bool isTag() const {
            return text.empty();
        }
    };
    
    void parse(const StringType& input) {
        using size_type = typename StringType::size_type;
        using streamstring = std::basic_ostringstream<typename StringType::value_type>;
        
        const StringType braceDelimiterBegin(2, '{');
        const StringType braceDelimiterEnd(2, '}');
        const StringType braceDelimiterEndUnescaped(3, '}');
        const size_type inputSize = input.size();
        
        StringType currentDelimiterBegin(braceDelimiterBegin);
        StringType currentDelimiterEnd(braceDelimiterEnd);
        bool currentDelimiterIsBrace = true;
        
        size_type inputPosition = 0;
        while (inputPosition != inputSize) {
            
            // Find the next tag start delimiter
            size_type tagLocationStart = input.find(currentDelimiterBegin, inputPosition);
            if (tagLocationStart == StringType::npos) {
                Component comp;
                comp.text = StringType(input, inputPosition, inputSize - inputPosition);
                rootComponent_.children.push_back(comp);
                break;
            } else if (tagLocationStart != inputPosition) {
                Component comp;
                comp.text = StringType(input, inputPosition, tagLocationStart - inputPosition);
                rootComponent_.children.push_back(comp);
            }
            
            size_type tagContentsLocation = tagLocationStart + currentDelimiterBegin.size();
            
            // Find the next tag end delimiter
            const bool tagIsUnescapedVar = currentDelimiterIsBrace && tagLocationStart != (inputSize - 2) && input.at(tagContentsLocation) == braceDelimiterBegin.at(0);
            const StringType& currenttTagDelimiterEnd(tagIsUnescapedVar ? braceDelimiterEndUnescaped : currentDelimiterEnd);
            if (tagIsUnescapedVar) {
                ++tagContentsLocation;
            }
            size_type tagLocationEnd = input.find(currenttTagDelimiterEnd, tagContentsLocation);
            if (tagLocationEnd == StringType::npos) {
                streamstring ss;
                ss << "No tag end delimiter found for start delimiter at: " << tagLocationStart;
                errorMessage_.assign(ss.str());
                return;
            }
            
            // Create a Tag object representation
            StringType tagContents(trim(StringType(input, tagContentsLocation, tagLocationEnd - tagContentsLocation)));
            Component comp;
            parseTagContents(tagIsUnescapedVar, tagContents, comp.tag);
            rootComponent_.children.push_back(comp);
            
            inputPosition = tagLocationEnd + currenttTagDelimiterEnd.size();
        }
        
        for (const auto& c : rootComponent_.children) {
            if (c.isTag()) {
                std::cout << "TAG: " << c.tag.name << std::endl;
            } else {
                std::cout << "TEXT: " << c.text << std::endl;
            }
        }

        for (auto it = rootComponent_.children.begin(); it != rootComponent_.children.end(); ++it) {
            auto comp = *it;
            if (comp.isTag() && comp.tag.isSectionBegin()) {
                auto endIt = std::find_if(std::next(it), rootComponent_.children.end(), [&comp](const Component& c1) {
                    return c1.isTag() && c1.tag.isSectionEnd() && c1.tag.name == comp.tag.name;
                });
                if (endIt == rootComponent_.children.end()) {
                    std::cout << "End section for " << comp.tag.name << " not found" << std::endl;
                } else {
                    std::cout << "End section for " << comp.tag.name << " found!" << std::endl;
                }
            }
        }
    }
    
    void parseTagContents(bool isUnescapedVar, const StringType& contents, Tag& tag) {
        if (isUnescapedVar) {
            tag.type = Tag::Type::UnescapedVariable;
            tag.name = contents;
        } else if (contents.empty()) {
            // Tag default values apply
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
    
    //std::wstring werrMsg;
    //(void)parse(std::wstring(), werrMsg);
    
    std::string input = "Hello {{name}}! Today is {{dayOfWeek}}.{{#alive}}You're alive!.{{/alive}}fin{{ender}}hi";
    Mustache::Mustache<std::string> templ(input);
    if (!templ.isValid()) {
        std::cout << "ERROR: " << templ.errorMessage() << std::endl;
    } else {
        std::cout << "No error" << std::endl;
    }
    
    return 0;
}
