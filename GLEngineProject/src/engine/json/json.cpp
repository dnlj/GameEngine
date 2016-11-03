// RapidJSON
#include <rapidjson/error/error.h>
#include <rapidjson/error/en.h>

// Engine
#include <engine/json/json.hpp>
#include <engine/util/util.hpp>

namespace engine::json {
	void loadDocument(const std::string& path, rapidjson::Document& doc) {
		rapidjson::ParseResult result = doc.Parse(engine::util::loadFile(path).c_str());

		if (result.IsError()) {
			std::string error = "Unable to parse file \"" + path + "\". ";
			error += rapidjson::GetParseError_En(result.Code());
			error += "  (" + result.Offset();
			error += ")";

			engine_error(error);
		}
	}

	rapidjson::Value::MemberIterator getObject(rapidjson::Value::ValueType& object, const std::string& name) {
		auto it = object.FindMember(name.c_str());

		if (it == object.MemberEnd()) {
			std::string error = "Unable to find member \"" + name + "\".";
			engine_error(error);
		} else if (!it->value.IsObject()) {
			std::string error = "Member \"" + name + "\" is not of type Object.";
			engine_error(error);
		}

		return it;
	}

	rapidjson::Value::MemberIterator getString(rapidjson::Value::ValueType& object, const std::string& name) {
		auto it = object.FindMember(name.c_str());

		if (it == object.MemberEnd()) {
			std::string error = "Unable to find member \"" + name + "\".";
			engine_error(error);
		} else if (!it->value.IsString()) {
			std::string error = "Member \"" + name + "\" is not of type String.";
			engine_error(error);
		}

		return it;
	}

	rapidjson::Value::MemberIterator getBool(rapidjson::Value::ValueType& object, const std::string& name) {
		auto it = object.FindMember(name.c_str());

		if (it == object.MemberEnd()) {
			std::string error = "Unable to find member \"" + name + "\".";
			engine_error(error);
		} else if (!it->value.IsBool()) {
			std::string error = "Member \"" + name + "\" is not of type Bool.";
			engine_error(error);
		}

		return it;
	}
}