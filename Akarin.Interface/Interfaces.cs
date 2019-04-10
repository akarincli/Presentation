using System;

namespace Akarin.Interface
{
    public interface IApplication : IDisposable
    {
        void ResumeControl();
        IDeviceSelectionFilter CreateDeviceSelectionFilter();
        IDeviceSelector OpenDeviceSelection(IDeviceSelectionFilter filter);
        IWindow CreateWindow(WindowCreateInfo createInfo);
    }

    public struct WindowCreateInfo
    {
        public string Caption;
        public int Display;
        public int PositionX;
        public int PositionY;
        public int Width;
        public int Height;
    }
    
    public interface IWindow : IDisposable
    {
        void Show();
        void Hide();
        void Minimize();
        void Maximize();
        IDisplaySurface CreateDisplaySurface();
    }

    public interface IDeviceSelectionFilter : IDisposable
    {
        ulong MemoryLowerLimit { get; set; }
        uint Texture1DMaxDimension { get; set; }
        uint Texture2DMaxDimension { get; set; }
        uint Texture3DMaxDimension { get; set; }
        bool RequireSwapChainAndPresent { get; set; }
        bool RequireGraphics { get; set; }
        bool RequireCompute { get; set; }
        IDisplaySurface SurfaceAttachment { set; }
    }
    
    public interface IDisplaySurface : IDisposable
    {
    }

    public interface IDeviceSelector : IDisposable {
    }

    public interface IDevice : IDisposable
    {
        IDisplayContext CreateDisplayContext(IWindow window = null);
    }

    public interface IContext : IDisposable
    {
    }
    
    public interface IDisplayContext : IContext {
        IPipeline CreatePipeline(PipelineCreateInfo createInfo);
        IRenderer CreateRenderer(RendererCreateInfo createInfo);
    }

    public interface IMemoryResource : IDisposable
    {
        
    }

    public interface IClientReadMemoryResource : IMemoryResource
    {
        
    }

    public interface IClientWriteMemoryResource : IMemoryResource
    {
        
    }

    public struct PipelineCreateInfo
    {
            
    }
    
    public interface IPipeline: IDisposable

    {
        
    }

    public struct RendererCreateInfo
    {
        
    }

    public interface IRenderer: IDisposable
    {
        
    }
}