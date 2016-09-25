# WinRegWriteBlocker
Windows write blocker through Windows registry

Under Windows, if you want easily write block USB devices, you can edit the registry :

> [HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Control\StorageDevicePolicies]

> "WriteProtect"=dword:00000001

The value of "WriteProtect" is "1" if enabled, "0" if not. This software monitors change of this value and displays a specific systray icon depending of the state so you can directly determine if you're blocking USB devices of not.

If you right-click on the systray icon, you can disable protection for 1 minute (after that the write protection is enabled) of disable until the next time this software is started.

**Remark:** this "write blocker software" only works for USB devices (and will not block eSata or firewire devices)
