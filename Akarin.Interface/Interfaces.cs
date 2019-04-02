using System;

namespace Akarin.Interface
{
    public interface IApplication : IDisposable
    {
        IWindow CreateWindow(WindowCreateInfo createInfo);
    }

    public struct WindowCreateInfo
    {
        
    }
    
    public interface IWindow : IDisposable
    {
        void Show();
        void Hide();
        void Close();
        void Minimize();
        void Maximize();
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