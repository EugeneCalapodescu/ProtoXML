#include <stdio.h>
#include <stdlib.h>

#include <sstream>

#include "..\proto\generated\Tests.pb.h"

#include "..\include\protoxml.h"

int main(int argc, char **argv)
{
	Person tom;

	tom.set_name("Tom");
	tom.set_type(Person::Type::Person_Type_EMPLOYEE);

	tom.mutable_address()->set_country("USA");
	tom.mutable_address()->set_city("NYC");
	tom.mutable_address()->add_detail("State: New York");
	tom.mutable_address()->add_detail("Road: West 56 St.");

	auto phone = tom.add_phone_number();
	phone->set_type(PhoneNumber::Type::PhoneNumber_Type_PRIVATE);
	phone->set_number("123.456.789");

	phone = tom.add_phone_number();
	phone->set_type(PhoneNumber::Type::PhoneNumber_Type_WORK);
	phone->set_number("xxx.yyy.zzz");

	PhoneNumber *secret_phone = new PhoneNumber();
	secret_phone->set_type(PhoneNumber::Type::PhoneNumber_Type_WORK);
	secret_phone->set_number("Secret xxx.xxx.xxx.xxx");

	tom.set_allocated_secret_phone(secret_phone);

	std::cout << "<!--\n" << tom.DebugString() << "-->\n";
	std::cout << SerializeAsXML(tom) << "\n";

	tom.set_double_choice(33);
	std::cout << "<!--\n" << tom.DebugString() << "-->\n";
	std::cout << SerializeAsXML(tom, "Tom") << "\n";

	Address france;
	france.set_country("France");
	france.set_city("Paris");
	france.add_detail("13 eme Arrondissement");
	france.add_detail("Avenue d'Italie");
	(*tom.mutable_other_addresses())["1"] = france;

	Address italy;
	italy.set_country("Italy");
	italy.set_city("Roma");
	(*tom.mutable_other_addresses())["2"] = italy;

	tom.set_bool_choice(false);

	std::cout << "<!--\n" << tom.DebugString() << "-->\n";
	std::cout << SerializeAsXML(tom) << "\n";
}
