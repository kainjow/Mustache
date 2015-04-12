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
#include <unordered_map>
#include <boost/any.hpp>

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
class Data {
public:
    enum class Type {
        Object,
        String,
        List,
        True,
        False,
    };
    
    using ObjectType = std::unordered_map<StringType, Data>;
    using ListType = std::vector<Data>;
    
    // Creation
    Data() : Data(Type::Object) {
    }
    Data(const StringType& string) : type_(Type::String) {
        data_ = string;
    }
    Data(const typename StringType::value_type* string) : type_(Type::String) {
        data_ = StringType(string);
    }
    Data(const ListType& list) : type_(Type::List) {
        data_ = list;
    }
    Data(bool boolean) : type_(Type::Bool) {
        data_ = boolean;
    }
    Data(Type type) : type_(type) {
        switch (type_) {
            case Type::Object:
                data_ = ObjectType();
                break;
            case Type::String:
                data_ = StringType();
                break;
            case Type::List:
                data_ = ListType();
                break;
            default:
                break;
        }
    }
    
    // Type info
    Type type() const {
        return type_;
    }
    bool isObject() const {
        return type_ == Type::Object;
    }
    bool isString() const {
        return type_ == Type::String;
    }
    bool isList() const {
        return type_ == Type::List;
    }
    bool isBool() const {
        return type_ == Type::True || type_ == Type::False;
    }
    bool isTrue() const {
        return type_ == Type::True;
    }
    bool isFalse() const {
        return type_ == Type::False;
    }
    
    // Object data
    void set(const StringType& name, const Data& var) {
        ObjectType& obj = boost::any_cast<ObjectType&>(data_);
        obj[name] = var;
    }
    bool exists(const StringType& name) {
        if (isObject()) {
            ObjectType& obj(boost::any_cast<ObjectType>(data_));
            if (obj.find(name) == obj.end()) {
                return true;
            }
        }
        return false;
    }
    bool get(const StringType& name, Data& var) const {
        if (!isObject()) {
            return false;
        }
        const ObjectType& obj(boost::any_cast<ObjectType>(data_));
        auto it = obj.find(name);
        if (it == obj.end()) {
            return false;
        }
        var = it->second;
        return true;
    }
#if 0
    const Data& operator[] (const StringType& name) const {
        ObjectType& obj(boost::any_cast<ObjectType>(data_));
        return obj[name];
    }
#endif
#if 0
    class ObjectVariableProxy {
    public:
        ObjectVariableProxy(Data& parent, const StringType& name)
            : parent_(parent)
            , name_(name)
        {}
        operator Data() {
            return parent_;
        }
        Data& operator= (const Data& var) {
            if (var != *this) {
                parent_.set(name_, var);
            }
            return *this;
        }
    private:
        Data& parent_;
        const StringType& name_;
    };
    ObjectVariableProxy& operator[] (const StringType& name) {
        return ObjectVariableProxy(*this, name);
    }
#endif
    
    // List data
    void push_back(const Data& var) {
        boost::any_cast<ListType&>(data_).push_back(var);
    }
    const Data& operator[] (size_t i) const {
        return boost::any_cast<const ListType&>(data_)[i];
    }
    const ListType& list() const {
        return boost::any_cast<const ListType&>(data_);
    }
    bool isEmptyList() {
        return isList() && boost::any_cast<const ListType&>(data_).empty();
    }
    
    // String data
    const StringType& stringValue() const {
        return boost::any_cast<const StringType&>(data_);
    }
    
private:
    Data* parent_;
    Type type_;
    boost::any data_;
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
    void render(OStream& stream, const Data<StringType>& data) {
        walk([&stream, &data, this](Component& comp, int) -> WalkControl {
            if (comp.isText()) {
                stream << comp.text;
            } else if (comp.isTag()) {
                const Tag& tag(comp.tag);
                Data<StringType> var;
                switch (tag.type) {
                    case Tag::Type::Variable: {
                        if (data.get(tag.name, var)) {
                            if (var.isString()) {
                                // TODO: escape
                                stream << var.stringValue();
                            } else if (var.isBool()) {
                                stream << (var.isTrue() ? StringType("true") : StringType("false"));
                            }
                        }
                        break;
                    }
                    case Tag::Type::UnescapedVariable:
                        if (data.get(tag.name, var)) {
                            if (var.isString()) {
                                stream << var.stringValue();
                            } else if (var.isBool()) {
                                stream << (var.isTrue() ? StringType("true") : StringType("false"));
                            }
                        }
                        break;
                    case Tag::Type::SectionBegin:
                        if (!data.get(tag.name, var) || var.isFalse() || var.isEmptyList()) {
                        } else {
                            renderSection(stream, data, comp, var);
                        }
                        return WalkControl::Skip;
                    case Tag::Type::SectionBeginInverted:
                        if (!data.get(tag.name, var) || var.isFalse() || var.isEmptyList()) {
                        } else {
                            return WalkControl::Skip;
                        }
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
    
    template <typename OStream>
    void print(OStream& stream) {
        walk([&stream](Component& comp, int depth) -> WalkControl {
            const StringType indent = depth >= 1 ? StringType(depth, ' ') : StringType();
            if (comp.isTag()) {
                stream << indent << "TAG: {{" << comp.tag.name << "}}" << std::endl;
            } else {
                stream << indent << "TXT: " << comp.text << std::endl;
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
                // No tag found. Add the remaining text.
                Component comp;
                comp.text = StringType(input, inputPosition, inputSize - inputPosition);
                comp.position = inputPosition;
                sections.back()->children.push_back(comp);
                break;
            } else if (tagLocationStart != inputPosition) {
                // Tag found, add text up to this tag.
                Component comp;
                comp.text = StringType(input, inputPosition, tagLocationStart - inputPosition);
                comp.position = inputPosition;
                sections.back()->children.push_back(comp);
            }
            
            // Find the next tag end delimiter
            StringSizeType tagContentsLocation = tagLocationStart + currentDelimiterBegin.size();
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
            
            // Push or pop sections
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
            
            // Start next search after this tag
            inputPosition = tagLocationEnd + currenttTagDelimiterEnd.size();
        }
        
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
    
    template <typename OStream>
    void renderSection(OStream& stream, const Data<StringType>& data, Component& incomp, const Data<StringType>& var) const {
        auto callback = [&stream, &data, &var](Component&, int) -> WalkControl {
            if (var.isList()) {
                for (auto item : var.list()) {
                    (void)item;
                }
            }
            return WalkControl::Skip;
        };
        walk(callback, incomp, 0);
    }

private:
    StringType errorMessage_;
    Component rootComponent_;
};

} // namespace

int main() {
    
    //Mustache::Mustache<std::wstring> templw(L"");
    
    using Data = Mustache::Data<std::string>;
    Data data;
    data.set("name", "Kevin");
    data.set("dayOfWeek", "Monday");
    Data list(Data::Type::List);
    for (auto &name : {"Peter", "Paul", "Mary"}) {
        Data item;
        item.set("name", name);
        list.push_back(item);
    }
    data.set("names", list);

    std::string input = "{{#names}}secret::{{/names}}Hello {{name}}! Today is {{dayOfWeek}}.";
    Mustache::Mustache<std::string> templ(input);
    if (!templ.isValid()) {
        std::cout << "ERROR: " << templ.errorMessage() << std::endl;
        return 0;
    }
    
    std::stringstream ss;
    templ.render(ss, data);
    std::cout << ss.str() << std::endl;
    
    return 0;
}
