#pragma once

// STD
#include<string>

// RapidJSON
#include <rapidjson/document.h>


namespace engine::json {
	void loadDocument(const std::string& path, rapidjson::Document& doc);
	rapidjson::Value::MemberIterator getObject(rapidjson::Value::ValueType& doc, const std::string& name);
	rapidjson::Value::MemberIterator getString(rapidjson::Value::ValueType& doc, const std::string& name);
}