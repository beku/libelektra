/**
 * \file
 *
 * \brief Tests for the Backend class
 *
 * \copyright BSD License (see doc/COPYING or http://www.libelektra.org)
 *
 */


#include <backend.hpp>
#include <backends.hpp>

#include <iostream>
#include <string>
#include <kdb.hpp>
#include <gtest/gtest.h>


std::string makeLiteralString(std::string str)
{
	std::string ret;
	for (size_t i=0; i<str.length(); ++i)
	{
		if (str[i] == '\\')
		{
			ret += "\\\\";
		} else {
			ret += str[i];
		}
	}
	return ret;
}

/**
 * @brief Easily allows one to generate regression tests for keysets.
 *
 * @param tocheck the keyset to check (name + string)
 * @param name the name of the keyset
 */
void outputGTest(kdb::KeySet tocheck, std::string name)
{
	std::cout << name << ".rewind();" << std::endl;
	tocheck.rewind();
	while(tocheck.next())
	{
		std::cout << name << ".next();" << std::endl;
		std::cout << "EXPECT_EQ(" << name
			<< ".current().getName(), \""
			<< makeLiteralString(tocheck.current().getName())
			<< "\") << \"name of element in keyset wrong\";"
			<< std::endl;
		std::cout << "EXPECT_EQ(" << name
			<< ".current().getString(), \""
			<< makeLiteralString(tocheck.current().getString())
			<< "\") << \"string of element in keyset wrong\";"
			<< std::endl;
	}
}

TEST(Backend, backendName)
{
	using namespace kdb;
	using namespace kdb::tools;
	Backend b;
	b.setMountpoint(Key("user/a", KEY_CASCADING_NAME, KEY_END), KeySet(0, KS_END));
	EXPECT_EQ(b.getMountpoint(), "user/a");
}

TEST(Backend, SimpleBackend)
{
	using namespace kdb;
	using namespace kdb::tools;
	Backend b;
	b.setMountpoint(Key("/", KEY_CASCADING_NAME, KEY_END), KeySet(0, KS_END));
	EXPECT_EQ(b.getMountpoint(), "/");
	b.addPlugin("resolver");
	b.addPlugin("dump");
	b.useConfigFile("abc");
	EXPECT_TRUE(b.validated());

	KeySet mountConfig;
	b.serialize(mountConfig);

	// outputGTest(mountConfig, "mountConfig");

	mountConfig.rewind();
	mountConfig.next();
	EXPECT_EQ(mountConfig.current().getName(), "system/elektra/mountpoints/\\/") << "name of element in keyset wrong";
	EXPECT_EQ(mountConfig.current().getString(), "This is a configuration for a backend, see subkeys for more information") << "string of element in keyset wrong";
	mountConfig.next();
	EXPECT_EQ(mountConfig.current().getName(), "system/elektra/mountpoints/\\//config") << "name of element in keyset wrong";
	EXPECT_EQ(mountConfig.current().getString(), "") << "string of element in keyset wrong";
	mountConfig.next();
	EXPECT_EQ(mountConfig.current().getName(), "system/elektra/mountpoints/\\//config/path") << "name of element in keyset wrong";
	EXPECT_EQ(mountConfig.current().getString(), "abc") << "string of element in keyset wrong";
	mountConfig.next();
	EXPECT_EQ(mountConfig.current().getName(), "system/elektra/mountpoints/\\//errorplugins") << "name of element in keyset wrong";
	EXPECT_EQ(mountConfig.current().getString(), "") << "string of element in keyset wrong";
	mountConfig.next();
	EXPECT_EQ(mountConfig.current().getName(), "system/elektra/mountpoints/\\//errorplugins/#5#resolver#resolver#") << "name of element in keyset wrong";
	EXPECT_EQ(mountConfig.current().getString(), "") << "string of element in keyset wrong";
	mountConfig.next();
	EXPECT_EQ(mountConfig.current().getName(), "system/elektra/mountpoints/\\//getplugins") << "name of element in keyset wrong";
	EXPECT_EQ(mountConfig.current().getString(), "") << "string of element in keyset wrong";
	mountConfig.next();
	EXPECT_EQ(mountConfig.current().getName(), "system/elektra/mountpoints/\\//getplugins/#0#resolver") << "name of element in keyset wrong";
	EXPECT_EQ(mountConfig.current().getString(), "") << "string of element in keyset wrong";
	mountConfig.next();
	EXPECT_EQ(mountConfig.current().getName(), "system/elektra/mountpoints/\\//getplugins/#5#dump#dump#") << "name of element in keyset wrong";
	EXPECT_EQ(mountConfig.current().getString(), "") << "string of element in keyset wrong";
	mountConfig.next();
	EXPECT_EQ(mountConfig.current().getName(), "system/elektra/mountpoints/\\//mountpoint") << "name of element in keyset wrong";
	EXPECT_EQ(mountConfig.current().getString(), "/") << "string of element in keyset wrong";
	mountConfig.next();
	EXPECT_EQ(mountConfig.current().getName(), "system/elektra/mountpoints/\\//setplugins") << "name of element in keyset wrong";
	EXPECT_EQ(mountConfig.current().getString(), "") << "string of element in keyset wrong";
	mountConfig.next();
	EXPECT_EQ(mountConfig.current().getName(), "system/elektra/mountpoints/\\//setplugins/#0#resolver") << "name of element in keyset wrong";
	EXPECT_EQ(mountConfig.current().getString(), "") << "string of element in keyset wrong";
	mountConfig.next();
	EXPECT_EQ(mountConfig.current().getName(), "system/elektra/mountpoints/\\//setplugins/#5#dump") << "name of element in keyset wrong";
	EXPECT_EQ(mountConfig.current().getString(), "") << "string of element in keyset wrong";
	mountConfig.next();
	EXPECT_EQ(mountConfig.current().getName(), "system/elektra/mountpoints/\\//setplugins/#7#resolver") << "name of element in keyset wrong";
	EXPECT_EQ(mountConfig.current().getString(), "") << "string of element in keyset wrong";
}

TEST(Backend, CrazyName)
{
	using namespace kdb;
	using namespace kdb::tools;
	Backend b;
	b.setMountpoint(Key("/crazy///.//name/../a..__.b/._.///._c__d", KEY_CASCADING_NAME, KEY_END), KeySet(0, KS_END));
	b.addPlugin("resolver");
	b.addPlugin("dump");
	EXPECT_TRUE(b.validated());

	KeySet mountConfig;
	b.serialize(mountConfig);

	// outputGTest(mountConfig, "mountConfig");

	mountConfig.rewind();
	mountConfig.next();
	EXPECT_EQ(mountConfig.current().getName(), "system/elektra/mountpoints/\\/crazy\\/a..__.b\\/._.\\/._c__d") << "name of element in keyset wrong";
	EXPECT_EQ(mountConfig.current().getString(), "This is a configuration for a backend, see subkeys for more information") << "string of element in keyset wrong";
	mountConfig.next();
	EXPECT_EQ(mountConfig.current().getName(), "system/elektra/mountpoints/\\/crazy\\/a..__.b\\/._.\\/._c__d/config") << "name of element in keyset wrong";
	EXPECT_EQ(mountConfig.current().getString(), "") << "string of element in keyset wrong";
	mountConfig.next();
	EXPECT_EQ(mountConfig.current().getName(), "system/elektra/mountpoints/\\/crazy\\/a..__.b\\/._.\\/._c__d/config/path") << "name of element in keyset wrong";
	EXPECT_EQ(mountConfig.current().getString(), "") << "string of element in keyset wrong";
	mountConfig.next();
	EXPECT_EQ(mountConfig.current().getName(), "system/elektra/mountpoints/\\/crazy\\/a..__.b\\/._.\\/._c__d/errorplugins") << "name of element in keyset wrong";
	EXPECT_EQ(mountConfig.current().getString(), "") << "string of element in keyset wrong";
	mountConfig.next();
	EXPECT_EQ(mountConfig.current().getName(), "system/elektra/mountpoints/\\/crazy\\/a..__.b\\/._.\\/._c__d/errorplugins/#5#resolver#resolver#") << "name of element in keyset wrong";
	EXPECT_EQ(mountConfig.current().getString(), "") << "string of element in keyset wrong";
	mountConfig.next();
	EXPECT_EQ(mountConfig.current().getName(), "system/elektra/mountpoints/\\/crazy\\/a..__.b\\/._.\\/._c__d/getplugins") << "name of element in keyset wrong";
	EXPECT_EQ(mountConfig.current().getString(), "") << "string of element in keyset wrong";
	mountConfig.next();
	EXPECT_EQ(mountConfig.current().getName(), "system/elektra/mountpoints/\\/crazy\\/a..__.b\\/._.\\/._c__d/getplugins/#0#resolver") << "name of element in keyset wrong";
	EXPECT_EQ(mountConfig.current().getString(), "") << "string of element in keyset wrong";
	mountConfig.next();
	EXPECT_EQ(mountConfig.current().getName(), "system/elektra/mountpoints/\\/crazy\\/a..__.b\\/._.\\/._c__d/getplugins/#5#dump#dump#") << "name of element in keyset wrong";
	EXPECT_EQ(mountConfig.current().getString(), "") << "string of element in keyset wrong";
	mountConfig.next();
	EXPECT_EQ(mountConfig.current().getName(), "system/elektra/mountpoints/\\/crazy\\/a..__.b\\/._.\\/._c__d/mountpoint") << "name of element in keyset wrong";
	EXPECT_EQ(mountConfig.current().getString(), "/crazy/a..__.b/._./._c__d") << "string of element in keyset wrong";
	mountConfig.next();
	EXPECT_EQ(mountConfig.current().getName(), "system/elektra/mountpoints/\\/crazy\\/a..__.b\\/._.\\/._c__d/setplugins") << "name of element in keyset wrong";
	EXPECT_EQ(mountConfig.current().getString(), "") << "string of element in keyset wrong";
	mountConfig.next();
	EXPECT_EQ(mountConfig.current().getName(), "system/elektra/mountpoints/\\/crazy\\/a..__.b\\/._.\\/._c__d/setplugins/#0#resolver") << "name of element in keyset wrong";
	EXPECT_EQ(mountConfig.current().getString(), "") << "string of element in keyset wrong";
	mountConfig.next();
	EXPECT_EQ(mountConfig.current().getName(), "system/elektra/mountpoints/\\/crazy\\/a..__.b\\/._.\\/._c__d/setplugins/#5#dump") << "name of element in keyset wrong";
	EXPECT_EQ(mountConfig.current().getString(), "") << "string of element in keyset wrong";
	mountConfig.next();
	EXPECT_EQ(mountConfig.current().getName(), "system/elektra/mountpoints/\\/crazy\\/a..__.b\\/._.\\/._c__d/setplugins/#7#resolver") << "name of element in keyset wrong";
	EXPECT_EQ(mountConfig.current().getString(), "") << "string of element in keyset wrong";
}

TEST(Backend, SimpleBackendWithConf)
{
	using namespace kdb;
	using namespace kdb::tools;
	Backend b;
	b.setMountpoint(Key("user/somewhere", KEY_END), KeySet(0, KS_END));
	EXPECT_EQ(b.getMountpoint(), "user/somewhere");
	KeySet backendConf(5,
		*Key("system/globalConf", KEY_VALUE, "for everywhere", KEY_END),
		*Key("system/other_global_conf", KEY_VALUE, "more", KEY_END),
		KS_END);
	b.setBackendConfig(backendConf);
	KeySet resConf(5,
		*Key("user/res_conf", KEY_VALUE, "do resolving", KEY_END),
		*Key("user/other_res_conf", KEY_VALUE, "do resolving too", KEY_END),
		KS_END);
	b.addPlugin("resolver", resConf);
	KeySet dumpConf(5,
		*Key("user/file_format", KEY_VALUE, "1", KEY_END),
		*Key("user/other_dump_conf", KEY_VALUE, "some dump config", KEY_END),
		KS_END);
	b.addPlugin("dump", dumpConf);
	b.useConfigFile("abc");
	EXPECT_TRUE(b.validated());

	KeySet mountConfig;
	b.serialize(mountConfig);

	// outputGTest(mountConfig, "mountConfig");

	mountConfig.rewind();
	mountConfig.next();
	EXPECT_EQ(mountConfig.current().getName(), "system/elektra/mountpoints/user\\/somewhere") << "name of element in keyset wrong";
	EXPECT_EQ(mountConfig.current().getString(), "This is a configuration for a backend, see subkeys for more information") << "string of element in keyset wrong";
	mountConfig.next();
	EXPECT_EQ(mountConfig.current().getName(), "system/elektra/mountpoints/user\\/somewhere/config") << "name of element in keyset wrong";
	EXPECT_EQ(mountConfig.current().getString(), "") << "string of element in keyset wrong";
	mountConfig.next();
	EXPECT_EQ(mountConfig.current().getName(), "system/elektra/mountpoints/user\\/somewhere/config/globalConf") << "name of element in keyset wrong";
	EXPECT_EQ(mountConfig.current().getString(), "for everywhere") << "string of element in keyset wrong";
	mountConfig.next();
	EXPECT_EQ(mountConfig.current().getName(), "system/elektra/mountpoints/user\\/somewhere/config/other_global_conf") << "name of element in keyset wrong";
	EXPECT_EQ(mountConfig.current().getString(), "more") << "string of element in keyset wrong";
	mountConfig.next();
	EXPECT_EQ(mountConfig.current().getName(), "system/elektra/mountpoints/user\\/somewhere/config/path") << "name of element in keyset wrong";
	EXPECT_EQ(mountConfig.current().getString(), "abc") << "string of element in keyset wrong";
	mountConfig.next();
	EXPECT_EQ(mountConfig.current().getName(), "system/elektra/mountpoints/user\\/somewhere/errorplugins") << "name of element in keyset wrong";
	EXPECT_EQ(mountConfig.current().getString(), "") << "string of element in keyset wrong";
	mountConfig.next();
	EXPECT_EQ(mountConfig.current().getName(), "system/elektra/mountpoints/user\\/somewhere/errorplugins/#5#resolver#resolver#") << "name of element in keyset wrong";
	EXPECT_EQ(mountConfig.current().getString(), "") << "string of element in keyset wrong";
	mountConfig.next();
	EXPECT_EQ(mountConfig.current().getName(), "system/elektra/mountpoints/user\\/somewhere/errorplugins/#5#resolver#resolver#/config") << "name of element in keyset wrong";
	EXPECT_EQ(mountConfig.current().getString(), "") << "string of element in keyset wrong";
	mountConfig.next();
	EXPECT_EQ(mountConfig.current().getName(), "system/elektra/mountpoints/user\\/somewhere/errorplugins/#5#resolver#resolver#/config/other_res_conf") << "name of element in keyset wrong";
	EXPECT_EQ(mountConfig.current().getString(), "do resolving too") << "string of element in keyset wrong";
	mountConfig.next();
	EXPECT_EQ(mountConfig.current().getName(), "system/elektra/mountpoints/user\\/somewhere/errorplugins/#5#resolver#resolver#/config/res_conf") << "name of element in keyset wrong";
	EXPECT_EQ(mountConfig.current().getString(), "do resolving") << "string of element in keyset wrong";
	mountConfig.next();
	EXPECT_EQ(mountConfig.current().getName(), "system/elektra/mountpoints/user\\/somewhere/getplugins") << "name of element in keyset wrong";
	EXPECT_EQ(mountConfig.current().getString(), "") << "string of element in keyset wrong";
	mountConfig.next();
	EXPECT_EQ(mountConfig.current().getName(), "system/elektra/mountpoints/user\\/somewhere/getplugins/#0#resolver") << "name of element in keyset wrong";
	EXPECT_EQ(mountConfig.current().getString(), "") << "string of element in keyset wrong";
	mountConfig.next();
	EXPECT_EQ(mountConfig.current().getName(), "system/elektra/mountpoints/user\\/somewhere/getplugins/#5#dump#dump#") << "name of element in keyset wrong";
	EXPECT_EQ(mountConfig.current().getString(), "") << "string of element in keyset wrong";
	mountConfig.next();
	EXPECT_EQ(mountConfig.current().getName(), "system/elektra/mountpoints/user\\/somewhere/getplugins/#5#dump#dump#/config") << "name of element in keyset wrong";
	EXPECT_EQ(mountConfig.current().getString(), "") << "string of element in keyset wrong";
	mountConfig.next();
	EXPECT_EQ(mountConfig.current().getName(), "system/elektra/mountpoints/user\\/somewhere/getplugins/#5#dump#dump#/config/file_format") << "name of element in keyset wrong";
	EXPECT_EQ(mountConfig.current().getString(), "1") << "string of element in keyset wrong";
	mountConfig.next();
	EXPECT_EQ(mountConfig.current().getName(), "system/elektra/mountpoints/user\\/somewhere/getplugins/#5#dump#dump#/config/other_dump_conf") << "name of element in keyset wrong";
	EXPECT_EQ(mountConfig.current().getString(), "some dump config") << "string of element in keyset wrong";
	mountConfig.next();
	EXPECT_EQ(mountConfig.current().getName(), "system/elektra/mountpoints/user\\/somewhere/mountpoint") << "name of element in keyset wrong";
	EXPECT_EQ(mountConfig.current().getString(), "user/somewhere") << "string of element in keyset wrong";
	mountConfig.next();
	EXPECT_EQ(mountConfig.current().getName(), "system/elektra/mountpoints/user\\/somewhere/setplugins") << "name of element in keyset wrong";
	EXPECT_EQ(mountConfig.current().getString(), "") << "string of element in keyset wrong";
	mountConfig.next();
	EXPECT_EQ(mountConfig.current().getName(), "system/elektra/mountpoints/user\\/somewhere/setplugins/#0#resolver") << "name of element in keyset wrong";
	EXPECT_EQ(mountConfig.current().getString(), "") << "string of element in keyset wrong";
	mountConfig.next();
	EXPECT_EQ(mountConfig.current().getName(), "system/elektra/mountpoints/user\\/somewhere/setplugins/#5#dump") << "name of element in keyset wrong";
	EXPECT_EQ(mountConfig.current().getString(), "") << "string of element in keyset wrong";
	mountConfig.next();
	EXPECT_EQ(mountConfig.current().getName(), "system/elektra/mountpoints/user\\/somewhere/setplugins/#7#resolver") << "name of element in keyset wrong";
	EXPECT_EQ(mountConfig.current().getString(), "") << "string of element in keyset wrong";
}
