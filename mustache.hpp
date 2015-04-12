//
//  Mustache
//  Created by Kevin Wojniak on 4/11/15.
//  Copyright (c) 2015 Kevin Wojniak. All rights reserved.
//

#ifndef MUSTACHE_HPP
#define MUSTACHE_HPP

#include <iostream>
#include <sstream>
#include <vector>
#include <functional>
#include <unordered_map>
#include <memory>

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
StringType escape(const StringType& s) {
    StringType ret;
    ret.reserve(s.size()*2);
    for (const auto ch : s) {
        switch (static_cast<char>(ch)) {
            case '&':
                ret.append("&amp;");
                break;
            case '<':
                ret.append("&lt;");
                break;
            case '>':
                ret.append("&gt;");
                break;
            case '\"':
                ret.append("&quot;");
                break;
            case '\'':
                ret.append("&apos;");
                break;
            default:
                ret.append(1, ch);
                break;
        }
    }
    return ret;
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
    
    // Construction
    Data() : Data(Type::Object) {
    }
    Data(const StringType& string) : type_(Type::String) {
        str_.reset(new StringType(string));
    }
    Data(const typename StringType::value_type* string) : type_(Type::String) {
        str_.reset(new StringType(string));
    }
    Data(const ListType& list) : type_(Type::List) {
        list_.reset(new ListType(list));
    }
    Data(Type type) : type_(type) {
        switch (type_) {
            case Type::Object:
                obj_.reset(new ObjectType());
                break;
            case Type::String:
                str_.reset(new StringType());
                break;
            case Type::List:
                list_.reset(new ListType());
                break;
            default:
                break;
        }
    }
    
    // Copying
    Data(const Data& data) : type_(data.type_) {
        if (data.obj_) {
            obj_.reset(new ObjectType(*data.obj_));
        } else if (data.str_) {
            str_.reset(new StringType(*data.str_));
        } else if (data.list_) {
            list_.reset(new ListType(*data.list_));
        }
    }
    Data& operator= (const Data& data) {
        if (&data != this) {
            type_ = data.type_;
            if (data.obj_) {
                obj_.reset(new ObjectType(*data.obj_));
            } else if (data.str_) {
                str_.reset(new StringType(*data.str_));
            } else if (data.list_) {
                list_.reset(new ListType(*data.list_));
            }
        }
        return *this;
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
        if (isObject()) {
            obj_->insert(std::pair<StringType,Data>(name, var));
        }
    }
    bool exists(const StringType& name) {
        if (isObject()) {
            if (obj_->find(name) == obj_->end()) {
                return true;
            }
        }
        return false;
    }
    bool get(const StringType& name, Data& var) const {
        if (!isObject()) {
            return false;
        }
        const auto& it = obj_->find(name);
        if (it == obj_->end()) {
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
        if (isList()) {
            list_->push_back(var);
        }
    }
    const Data& operator[] (size_t i) const {
        return list_->at(i);
    }
    const ListType& list() const {
        return *list_;
    }
    bool isEmptyList() {
        return isList() && list_->empty();
    }
    
    // String data
    const StringType& stringValue() const {
        return *str_;
    }

private:
    Type type_;
    std::unique_ptr<ObjectType> obj_;
    std::unique_ptr<StringType> str_;
    std::unique_ptr<ListType> list_;
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
                                stream << escape(var.stringValue());
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
    
    StringType render(const Data<StringType>& data) {
        using streamstring = std::basic_ostringstream<typename StringType::value_type>;
        streamstring ss;
        render(ss, data);
        return ss.str();
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
                for (auto& item : var.list()) {
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

#endif // MUSTACHE_HPP
