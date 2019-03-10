#pragma once

#include <google\protobuf\message.h>

std::string SerializeAsXML(const google::protobuf::Message& message, const std::string& name = "");


