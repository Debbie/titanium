/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */

#include <kroll/kroll.h>
#include <Poco/Environment.h>
#include <Poco/UUIDGenerator.h>
#include <Poco/Net/NetworkInterface.h>
#include <Poco/Net/IPAddress.h>
#include "platform_binding.h"

#ifdef OS_OSX
#include <Foundation/Foundation.h>
#elif defined(OS_WIN32)
#include <windows.h>
#include <Iptypes.h>
#include <Iphlpapi.h>
#elif defined(OS_LINUX)
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sstream>
#endif

#if defined(OS_OSX)
@interface NSProcessInfo (LegacyWarningSurpression)
- (unsigned int) processorCount;
//TODO: do the smart thing and test for it being NSUinteger
@end
#endif


namespace ti
{
	PlatformBinding::PlatformBinding(SharedKObject global) : global(global)
	{
		std::string os_name = Poco::Environment::osName();
		std::string os_version = Poco::Environment::osVersion();
		std::string arch = Poco::Environment::osArchitecture();
		std::string address = "127.0.0.1";

		std::vector<Poco::Net::NetworkInterface> list = Poco::Net::NetworkInterface::list();
		std::vector<Poco::Net::NetworkInterface>::iterator i = list.begin();
		SharedKList interfaces = new StaticBoundList();
		int c = 0;
		while (i!=list.end())
		{
			Poco::Net::NetworkInterface nitf = (*i++);
			if (nitf.supportsIPv4())
			{
				const Poco::Net::IPAddress ip = nitf.address();
				if (ip.isLoopback() || !ip.isIPv4Compatible())
				{
					continue;
				}
				c++;
				// just get the first one and bail
				if (c==1) address = ip.toString();
				// add each interface
				SharedKObject obj = new StaticBoundObject();
				std::string ip_addr = ip.toString();
				std::string display_name = nitf.displayName();
				std::string name = nitf.name();
				/**
				 * @tiapi(property=True,type=string,name=Platform.Interface.address,since=0.2) ip address
				 */
				obj->Set("address",Value::NewString(ip_addr));
				/**
				 * @tiapi(property=True,type=string,name=Platform.Interface.name,since=0.2) name of the interface
				 */
				obj->Set("name",Value::NewString(name));
				/**
				 * @tiapi(property=True,type=string,name=Platform.Interface.displayName,since=0.2) display name of the interface
				 */
				obj->Set("displayName",Value::NewString(display_name));
				interfaces->Append(Value::NewObject(obj));
			}
		}
		/**
		 * @tiapi(method=True,type=list,name=Platform.interfaces,since=0.3) returns a list of network interfaces
		 */
		this->Set("interfaces", Value::NewList(interfaces));


#if defined(OS_OSX)
		int num_proc = 1;
		if ([NSProcessInfo instancesRespondToSelector:@selector(processorCount)]){
			num_proc = 	[[NSProcessInfo processInfo] processorCount];
		}
#elif defined(OS_WIN32)
		SYSTEM_INFO SysInfo ;
		GetSystemInfo (&SysInfo) ;
		DWORD num_proc = SysInfo.dwNumberOfProcessors;
#elif defined(OS_LINUX)
		int num_proc = sysconf(_SC_NPROCESSORS_ONLN);
#else
		int num_proc = 1;
#endif

		std::string machineid = PlatformUtils::GetMachineId();
		std::string macAddress = PlatformUtils::GetFirstMACAddress();

//NOTE: for now we determine this at compile time -- in the future
//we might want to actually programmatically determine if running on
//64-bit processor or not...
		/**
		 * @tiapi(property=True,type=string,name=Platform.ostype,since=0.3) the architecture type (either 32 bit or 64 bit)
		 */
#ifdef OS_32
		this->Set("ostype", Value::NewString("32bit"));
#else
		this->Set("ostype", Value::NewString("64bit"));
#endif
		/**
		 * @tiapi(property=True,type=string,name=Platform.name,since=0.2) the operating system name
		 */
		this->Set("name", Value::NewString(os_name));
		/**
		 * @tiapi(property=True,type=string,name=Platform.version,since=0.2) the operating system version
		 */
		this->Set("version", Value::NewString(os_version));
		/**
		 * @tiapi(property=True,type=string,name=Platform.architecture,since=0.2) the operating system architecture
		 */
		this->Set("architecture", Value::NewString(arch));
		/**
		 * @tiapi(property=True,type=string,name=Platform.address,since=0.2) the primary ip address
		 */
		this->Set("address", Value::NewString(address));
		/**
		 * @tiapi(property=True,type=string,name=Platform.id,since=0.2) the unique machine id
		 */
		this->Set("id", Value::NewString(machineid));
		/**
		 * @tiapi(property=True,type=string,name=Platform.macaddress,since=0.3) the primary mac address
		 */
		this->Set("macaddress", Value::NewString(macAddress));
		/**
		 * @tiapi(property=True,type=integer,name=Platform.processorCount,since=0.2) the number of processors for the machine
		 */
		this->Set("processorCount", Value::NewInt(num_proc));
		std::string username = kroll::FileUtils::GetUsername();
		/**
		 * @tiapi(property=True,type=string,name=Platform.username,since=0.2) the platform's user name
		 */
		this->Set("username", Value::NewString(username));

		// UUID create function for the platform
		/**
		 * @tiapi(method=True,returns=string,name=Platform.createUUID,since=0.3) create a globally unique id
		 * @tiresult(for=Platform.createUUID,type=string) result as string
		 */
		this->SetMethod("createUUID",&PlatformBinding::CreateUUID);
	}

	PlatformBinding::~PlatformBinding()
	{
	}

	void PlatformBinding::CreateUUID(const ValueList& args, SharedValue result)
	{
		Poco::UUID uuid = Poco::UUIDGenerator::defaultGenerator().createRandom();
		result->SetString(uuid.toString().c_str());
	}
}
