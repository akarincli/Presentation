namespace Akarin.Interface
{
    public class Vulkan : IApplication
    {
        private const string NativeLib = "AkarinNative";
        
        public void Dispose()
        {
            throw new System.NotImplementedException();
        }

        public IWindow CreateWindow(WindowCreateInfo createInfo)
        {
            return new SDLWindow(createInfo);
        }

        private class SDLWindow : IWindow
        {
            public SDLWindow(WindowCreateInfo createInfo)
            {
                throw new System.NotImplementedException();
            }

            public void Dispose()
            {
                throw new System.NotImplementedException();
            }

            public void Show()
            {
                throw new System.NotImplementedException();
            }

            public void Hide()
            {
                throw new System.NotImplementedException();
            }

            public void Close()
            {
                throw new System.NotImplementedException();
            }

            public void Minimize()
            {
                throw new System.NotImplementedException();
            }

            public void Maximize()
            {
                throw new System.NotImplementedException();
            }
        }
    }
}