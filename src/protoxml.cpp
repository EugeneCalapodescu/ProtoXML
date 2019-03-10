#include "google\protobuf\message.h"
#include "..\include\protoxml.h"

#include <sstream>

void print_message(const google::protobuf::Message& message, const std::string& name, std::ostream& xml);

enum AttributeOrElement
{
	ATTRIBUTE = 0,
	ELEMENT = 1
};

// Display only the oneof fields that is actually set
inline bool should_display(const google::protobuf::Message& message,
	const google::protobuf::FieldDescriptor& field,
	const google::protobuf::Reflection& reflection)
{
	const google::protobuf::OneofDescriptor* oneof = field.containing_oneof();
	if (!oneof)
		return true;

	return reflection.GetOneofFieldDescriptor(message, oneof) == &field;
}

void print_value(const google::protobuf::Message& message,
	const google::protobuf::FieldDescriptor& field,
	int index, // ignored for single ocurrence
	AttributeOrElement mode,
	const google::protobuf::Reflection& reflection,
	std::ostream& xml)
{
	if (!should_display(message, field, reflection))
		return;

	if (mode == ELEMENT)
		xml << "<" << field.name() << ">";
	else
		xml << " " << field.name() << "=\"";

	auto repeated = field.is_repeated();
	switch (auto type = field.type())
	{
	case google::protobuf::FieldDescriptor::TYPE_BOOL:
		// doesnt seems to be used
		xml << (repeated ?
			(reflection.GetRepeatedBool(message, &field, index) ? "true" : "false") :
			(reflection.GetBool(message, &field) ? "false" : "false"));
		break;

	case google::protobuf::FieldDescriptor::TYPE_INT32:
		xml << (repeated ?
			reflection.GetRepeatedInt32(message, &field, index) :
			reflection.GetInt32(message, &field));
		break;

	case google::protobuf::FieldDescriptor::TYPE_UINT32:
		xml << (repeated ?
			reflection.GetRepeatedUInt32(message, &field, index) :
			reflection.GetUInt32(message, &field));
		break;

	case google::protobuf::FieldDescriptor::TYPE_INT64:
		xml << (repeated ?
			reflection.GetRepeatedInt64(message, &field, index) :
			reflection.GetInt64(message, &field));
		break;

	case google::protobuf::FieldDescriptor::TYPE_UINT64:
		xml << (repeated ?
			reflection.GetRepeatedUInt64(message, &field, index) :
			reflection.GetUInt64(message, &field));
		break;

	case google::protobuf::FieldDescriptor::TYPE_DOUBLE:
		xml << (repeated ?
			reflection.GetRepeatedDouble(message, &field, index) :
			reflection.GetDouble(message, &field));
		break;

	case google::protobuf::FieldDescriptor::TYPE_FLOAT:
		xml << (repeated ?
			reflection.GetRepeatedFloat(message, &field, index) :
			reflection.GetFloat(message, &field));
		break;

	case google::protobuf::FieldDescriptor::TYPE_STRING:
		xml << (repeated ?
			reflection.GetRepeatedString(message, &field, index) :
			reflection.GetString(message, &field));
		break;

	case google::protobuf::FieldDescriptor::TYPE_BYTES:
		xml << "<!CDATA[" << (repeated ?
			reflection.GetRepeatedString(message, &field, index) :
			reflection.GetString(message, &field))
			<< "]]>";
		break;

	case google::protobuf::FieldDescriptor::TYPE_ENUM:
		xml << (repeated ?
			reflection.GetRepeatedEnum(message, &field, index)->name() :
			reflection.GetEnum(message, &field)->name());
		break;

	default:
		throw std::exception("Invalid value type.");
	}

	if (mode == ELEMENT)
		xml << "</" << field.name() << ">";
	else
		xml << "\"";
}

void print_element(const google::protobuf::Message& message,
	const google::protobuf::FieldDescriptor& field,
	const google::protobuf::Reflection& reflection,
	std::ostream& xml)
{
	if (!field.is_repeated())  // so its an embedded structure
	{
		if (should_display(message, field, reflection))
			print_message(reflection.GetMessage(message, &field), field.name(), xml);

		return;
	}

	for (int i = 0; i < reflection.FieldSize(message, &field); i++)
	{
		if (field.cpp_type() == google::protobuf::FieldDescriptor::CPPTYPE_MESSAGE)
			print_message(reflection.GetRepeatedMessage(message, &field, i), field.name(), xml);
		else
			print_value(message, field, i, ELEMENT, reflection, xml);
	}
}

void print_message(const google::protobuf::Message& message, const std::string& name, std::ostream& xml)
{
	const google::protobuf::Descriptor *descriptor = message.GetDescriptor();
	const google::protobuf::Reflection *reflection = message.GetReflection();

	xml << "<" << name;
	for (int i = 0; i < descriptor->field_count(); i++)
	{
		const auto field = descriptor->field(i);
		if (!field->is_repeated() && field->type() != google::protobuf::FieldDescriptor::TYPE_MESSAGE)
			print_value(message, *field, 0, ATTRIBUTE, *reflection, xml);
	}
	xml << ">";

	for (int i = 0; i < descriptor->field_count(); i++)
	{
		const auto field = descriptor->field(i);
		if (field->is_repeated() || field->type() == google::protobuf::FieldDescriptor::TYPE_MESSAGE)
			print_element(message, *field, *reflection, xml);
	}

	xml << "</" << name << ">" << "\n";
}

std::string SerializeAsXML(const google::protobuf::Message& message, const std::string& name)
{
	std::stringstream xml;

	print_message(message, name.empty() ? message.GetDescriptor()->name() : name, xml);
	return xml.str();
}
