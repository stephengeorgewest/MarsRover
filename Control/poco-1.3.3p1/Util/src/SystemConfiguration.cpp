//
// SystemConfiguration.cpp
//
// $Id: //poco/1.3/Util/src/SystemConfiguration.cpp#1 $
//
// Library: Util
// Package: Configuration
// Module:  SystemConfiguration
//
// Copyright (c) 2004-2006, Applied Informatics Software Engineering GmbH.
// and Contributors.
//
// Permission is hereby granted, free of charge, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, reproduce, display, distribute,
// execute, and transmit the Software, and to prepare derivative works of the
// Software, and to permit third-parties to whom the Software is furnished to
// do so, all subject to the following:
// 
// The copyright notices in the Software and this entire statement, including
// the above license grant, this restriction and the following disclaimer,
// must be included in all copies of the Software, in whole or in part, and
// all derivative works of the Software, unless such copies or derivative
// works are solely in the form of machine-executable object code generated by
// a source language processor.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//


#include "Poco/Util/SystemConfiguration.h"
#include "Poco/Environment.h"
#include "Poco/Path.h"
#include "Poco/Exception.h"


using Poco::Environment;
using Poco::Path;


namespace Poco {
namespace Util {


const std::string SystemConfiguration::OSNAME         = "system.osName";
const std::string SystemConfiguration::OSVERSION      = "system.osVersion";
const std::string SystemConfiguration::OSARCHITECTURE = "system.osArchitecture";
const std::string SystemConfiguration::NODENAME       = "system.nodeName";
const std::string SystemConfiguration::CURRENTDIR     = "system.currentDir";
const std::string SystemConfiguration::HOMEDIR        = "system.homeDir";
const std::string SystemConfiguration::TEMPDIR        = "system.tempDir";
const std::string SystemConfiguration::ENV            = "system.env.";


SystemConfiguration::SystemConfiguration()
{
}


SystemConfiguration::~SystemConfiguration()
{
}


bool SystemConfiguration::getRaw(const std::string& key, std::string& value) const
{
	if (key == OSNAME)
		value = Environment::osName();
	else if (key == OSVERSION)
		value = Environment::osVersion();
	else if (key == OSARCHITECTURE)
		value = Environment::osArchitecture();
	else if (key == NODENAME)
		value = Environment::nodeName();
	else if (key == CURRENTDIR)
		value = Path::current();
	else if (key == HOMEDIR)
		value = Path::home();
	else if (key == TEMPDIR)
		value = Path::temp();
	else if (key.compare(0, ENV.size(), ENV) == 0)
		return getEnv(key.substr(ENV.size()), value);
	else
		return false;
	return true;
}


void SystemConfiguration::setRaw(const std::string& key, const std::string& value)
{
	throw Poco::InvalidAccessException("Attempt to modify a system property", key);
}


void SystemConfiguration::enumerate(const std::string& key, Keys& range) const
{
	if (key.empty())
	{
		range.push_back("system");
	}
	else if (key == "system")
	{
		range.push_back("osName");
		range.push_back("osVersion");
		range.push_back("osArchitecture");
		range.push_back("nodeName");
		range.push_back("currentDir");
		range.push_back("homeDir");
		range.push_back("tempDir");
		range.push_back("env");
	}
}


bool SystemConfiguration::getEnv(const std::string& name, std::string& value)
{
	if (Environment::has(name))
	{
		value = Environment::get(name);
		return true;
	}
	return false;
}


} } // namespace Poco::Util
