#include "main.h"

CreateInterfaceFn CaptureFactory(Module* module)
{
	if (!module || !module->handle)
		return 0;

	return (CreateInterfaceFn)(GetProcAddress(module->handle, CREATEINTERFACE_PROCNAME));
}

void* CaptureInterface(CreateInterfaceFn Interface, const char* InterfaceName)
{
	if (Interface && InterfaceName)
		return (PVOID)(Interface(InterfaceName, 0));

	return 0;
}
