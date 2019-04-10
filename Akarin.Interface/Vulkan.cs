using System;
using System.Runtime.InteropServices;

namespace Akarin.Interface
{
    public class Vulkan : IApplication
    {
        private const string NativeLib = "AkarinNative";

        private static UIntPtr instanceHandle;

        [DllImport(NativeLib, EntryPoint = "akAppInit", CallingConvention = CallingConvention.Cdecl)]
        private static extern UIntPtr AkAppInit();

        [DllImport(NativeLib, EntryPoint = "akAppControlHandOver", CallingConvention = CallingConvention.Cdecl)]
        private static extern void AkAppControlHandOver(UIntPtr handle);

        [DllImport(NativeLib, EntryPoint = "akAppStop", CallingConvention = CallingConvention.Cdecl)]
        private static extern void AkAppStop(UIntPtr handle);

        [DllImport(NativeLib, EntryPoint = "akAppFinalize", CallingConvention = CallingConvention.Cdecl)]
        private static extern void AkAppFinalize(UIntPtr handle);

        public Vulkan()
        {
            instanceHandle = AkAppInit();
        }
        
        ~Vulkan()
        {
            Dispose();
        }
        
        public void Dispose()
        {
            GC.Collect(); // This class contains critical context info. just making sure everything is released
            GC.SuppressFinalize(this);
            AkAppFinalize(instanceHandle);
        }

        public void ResumeControl()
        {
            AkAppControlHandOver(instanceHandle);
        }

        public IDeviceSelectionFilter CreateDeviceSelectionFilter()
        {
            return new VkDeviceSelectionFilter();
        }

        public IDeviceSelector OpenDeviceSelection(IDeviceSelectionFilter filter)
        {
            return new VkDeviceSelector(filter as VkDeviceSelectionFilter);
        }

        public IWindow CreateWindow(WindowCreateInfo createInfo)
        {
            return new SDLWindow(createInfo);
        }

        private class SDLWindow : IWindow
        {
            [DllImport(NativeLib, EntryPoint = "akCreateWindow", CallingConvention = CallingConvention.Cdecl)]
            private static extern UIntPtr AkCreateWindow([MarshalAs(UnmanagedType.LPStr)] string caption, int display,
                int positionX, int positionY, int width, int height);

            [DllImport(NativeLib, EntryPoint = "akShowWindow", CallingConvention = CallingConvention.Cdecl)]
            private static extern void AkShowWindow(UIntPtr handle);

            [DllImport(NativeLib, EntryPoint = "akHideWindow", CallingConvention = CallingConvention.Cdecl)]
            private static extern void AkHideWindow(UIntPtr handle);

            [DllImport(NativeLib, EntryPoint = "akMinimizeWindow", CallingConvention = CallingConvention.Cdecl)]
            private static extern void AkMinimizeWindow(UIntPtr handle);

            [DllImport(NativeLib, EntryPoint = "akMaximizeWindow", CallingConvention = CallingConvention.Cdecl)]
            private static extern void AkMaximizeWindow(UIntPtr handle);

            [DllImport(NativeLib, EntryPoint = "akDestroyWindow", CallingConvention = CallingConvention.Cdecl)]
            private static extern void AkDestroyWindow(UIntPtr handle);
            
            public SDLWindow(WindowCreateInfo createInfo)
            {
                _handle = AkCreateWindow(createInfo.Caption, createInfo.Display, createInfo.PositionX,
                    createInfo.PositionY, createInfo.Width, createInfo.Height);
            }

            ~SDLWindow()
            {
                Dispose();
            }

            public void Dispose()
            {
                GC.SuppressFinalize(this);
                AkDestroyWindow(_handle);
            }

            public void Show()
            {
                AkShowWindow(_handle);
            }

            public void Hide()
            {
                AkHideWindow(_handle);
            }

            public void Minimize()
            {
                AkMinimizeWindow(_handle);
            }

            public void Maximize()
            {
                AkMaximizeWindow(_handle);
            }

            public IDisplaySurface CreateDisplaySurface()
            {
                return new SDLVkDisplaySurface(_handle);
            }

            private readonly UIntPtr _handle;
        }

        private class SDLVkDisplaySurface : IDisplaySurface
        {
            [DllImport(NativeLib, EntryPoint = "akCreateDisplaySurface", CallingConvention = CallingConvention.Cdecl)]
            private static extern UIntPtr AkCreateDisplaySurface(UIntPtr application, UIntPtr window);

            [DllImport(NativeLib, EntryPoint = "akDestroyDisplaySurface", CallingConvention = CallingConvention.Cdecl)]
            private static extern void AkDestroyDisplaySurface(UIntPtr application, UIntPtr surface);

            public SDLVkDisplaySurface(UIntPtr window)
            {
                _handle = AkCreateDisplaySurface(instanceHandle, window);
            }

            ~SDLVkDisplaySurface()
            {
                Dispose();
            }

            public void Dispose()
            {
                AkDestroyDisplaySurface(instanceHandle, _handle);
                GC.SuppressFinalize(this);
            }

            public UIntPtr GetNative()
            {
                return _handle;
            }
            
            private readonly UIntPtr _handle;
        }

        private class VkDeviceSelectionFilter : IDeviceSelectionFilter
        {
            [DllImport(NativeLib, EntryPoint = "akCreateDeviceSelectorFilter", CallingConvention = CallingConvention.Cdecl)]
            private static extern UIntPtr AkCreateDeviceSelectorFilter();

            [DllImport(NativeLib, EntryPoint = "akDeviceSelectorFilterSetBool", CallingConvention = CallingConvention.Cdecl)]
            private static extern void AkDeviceSelectorFilterSetBool(UIntPtr handle, int name, bool value);

            [DllImport(NativeLib, EntryPoint = "akDeviceSelectorFilterSetUInt", CallingConvention = CallingConvention.Cdecl)]
            private static extern void AkDeviceSelectorFilterSetUInt(UIntPtr handle, int name, uint value);

            [DllImport(NativeLib, EntryPoint = "akDeviceSelectorFilterSetUInt64", CallingConvention = CallingConvention.Cdecl)]
            private static extern void AkDeviceSelectorFilterSetUInt64(UIntPtr handle, int name, ulong value);

            [DllImport(NativeLib, EntryPoint = "akDeviceSelectorFilterSetDouble", CallingConvention = CallingConvention.Cdecl)]
            private static extern void AkDeviceSelectorFilterSetDouble(UIntPtr handle, int name, double value);

            [DllImport(NativeLib, EntryPoint = "akDeviceSelectorFilterSetHandle", CallingConvention = CallingConvention.Cdecl)]
            private static extern void AkDeviceSelectorFilterSetHandle(UIntPtr handle, int name, UIntPtr value);

            [DllImport(NativeLib, EntryPoint = "akDeviceSelectorFilterGetBool", CallingConvention = CallingConvention.Cdecl)]
            private static extern bool AkDeviceSelectorFilterGetBool(UIntPtr handle, int name);

            [DllImport(NativeLib, EntryPoint = "akDeviceSelectorFilterGetUInt", CallingConvention = CallingConvention.Cdecl)]
            private static extern uint AkDeviceSelectorFilterGetUInt(UIntPtr handle, int name);

            [DllImport(NativeLib, EntryPoint = "akDeviceSelectorFilterGetUInt64", CallingConvention = CallingConvention.Cdecl)]
            private static extern ulong AkDeviceSelectorFilterGetUInt64(UIntPtr handle, int name);

            [DllImport(NativeLib, EntryPoint = "akDeviceSelectorFilterGetDouble", CallingConvention = CallingConvention.Cdecl)]
            private static extern double AkDeviceSelectorFilterGetDouble(UIntPtr handle, int name);

            [DllImport(NativeLib, EntryPoint = "akDeviceSelectorFilterGetHandle", CallingConvention = CallingConvention.Cdecl)]
            private static extern UIntPtr AkDeviceSelectorFilterGetHandle(UIntPtr handle, int name);

            [DllImport(NativeLib, EntryPoint = "akDestroyDeviceSelectorFilter", CallingConvention = CallingConvention.Cdecl)]
            private static extern void AkDestroyDeviceSelectorFilter(UIntPtr handle);

            public VkDeviceSelectionFilter()
            {
                _handle = AkCreateDeviceSelectorFilter();
            }
            

            ~VkDeviceSelectionFilter()
            {
                Dispose();
            }
            
            public void Dispose()
            {
                AkDestroyDeviceSelectorFilter(_handle);
                GC.SuppressFinalize(this);
            }

            private enum Names
            {
                MemoryLowerLimit = 0,
                Texture1DMaxDimension = 0x100,
                Texture2DMaxDimension,
                Texture3DMaxDimension,
                RequireSwapChainAndPresent = 0x200,
                RequireGraphics,
                RequireCompute,
                SurfaceAttachment = 0x300
            }
            
            public ulong MemoryLowerLimit
            {
                get => AkDeviceSelectorFilterGetUInt64(_handle, (int) Names.MemoryLowerLimit);
                set => AkDeviceSelectorFilterSetUInt64(_handle, (int) Names.MemoryLowerLimit, value);
            }

            public uint Texture1DMaxDimension 
            {
                get => AkDeviceSelectorFilterGetUInt(_handle, (int) Names.Texture1DMaxDimension);
                set => AkDeviceSelectorFilterSetUInt(_handle, (int) Names.Texture1DMaxDimension, value);
            }
            
            public uint Texture2DMaxDimension 
            {
                get => AkDeviceSelectorFilterGetUInt(_handle, (int) Names.Texture2DMaxDimension);
                set => AkDeviceSelectorFilterSetUInt(_handle, (int) Names.Texture2DMaxDimension, value);
            }
            
            public uint Texture3DMaxDimension 
            {
                get => AkDeviceSelectorFilterGetUInt(_handle, (int) Names.Texture3DMaxDimension);
                set => AkDeviceSelectorFilterSetUInt(_handle, (int) Names.Texture3DMaxDimension, value);
            }
            
            public bool RequireSwapChainAndPresent 
            {
                get => AkDeviceSelectorFilterGetBool(_handle, (int) Names.RequireSwapChainAndPresent);
                set => AkDeviceSelectorFilterSetBool(_handle, (int) Names.RequireSwapChainAndPresent, value);
            }

            public bool RequireGraphics
            {
                get => AkDeviceSelectorFilterGetBool(_handle, (int) Names.RequireGraphics);
                set => AkDeviceSelectorFilterSetBool(_handle, (int) Names.RequireGraphics, value);
            }
            
            public bool RequireCompute 
            {
                get => AkDeviceSelectorFilterGetBool(_handle, (int) Names.RequireCompute);
                set => AkDeviceSelectorFilterSetBool(_handle, (int) Names.RequireCompute, value);
            }
            
            public IDisplaySurface SurfaceAttachment 
            {
                set => AkDeviceSelectorFilterSetHandle(_handle, (int) Names.SurfaceAttachment, 
                    (value as SDLVkDisplaySurface)?.GetNative() ?? UIntPtr.Zero);
            }

            public UIntPtr GetNative()
            {
                return _handle;
            }
            
            private readonly UIntPtr _handle;
        }

        private class VkDeviceSelector : IDeviceSelector
        {
            [DllImport(NativeLib, EntryPoint = "akFilterDevices", CallingConvention = CallingConvention.Cdecl)]
            private static extern UIntPtr AkFilterDevices(UIntPtr appHandle, UIntPtr filter);
            
            [DllImport(NativeLib, EntryPoint = "akReleaseDeviceFilterResults", CallingConvention = CallingConvention.Cdecl)]
            private static extern void AkReleaseDeviceFilterResults(UIntPtr handle);
            
            public VkDeviceSelector(VkDeviceSelectionFilter filters)
            {
                _handle = AkFilterDevices(instanceHandle, filters.GetNative());
            }
            
            ~VkDeviceSelector()
            {
                Dispose();
            }
            
            public void Dispose()
            {
                AkReleaseDeviceFilterResults(_handle);
                GC.SuppressFinalize(this);
            }
            
            private readonly UIntPtr _handle;
        }
    }
}