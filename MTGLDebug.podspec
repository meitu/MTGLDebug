Pod::Spec.new do |s|
  s.name         = "MTGLDebug"

  s.version      = "3.0.0"

  s.summary      = "An OpenGL Debugging tool for iOS"
  s.description  = "An OpenGL Debugging tool for iOS, provides the ability to track OpenGL resource memory usage and common incorrect usage of OpenGL API."
  s.homepage     = "https://github.com/meitu/MTGLDebug.git"
  s.license      = { 
  	:type => 'Copyright', 
  	:text => <<-LICENSE 
  			@ 2008 - present Meitu, Inc. All rights reserved.
  	LICENSE
  }

  s.authors      = { 'Xiaojun Zhang' => '124825961@qq.com' , 'wzq' => 'wzq@meitu.com' }
  s.platform     = :ios, '8.0'
  s.source       = { :git => "https://github.com/meitu/MTGLDebug.git", :tag => "#{s.version}" }
  

  s.subspec 'ARC' do |spec|
    spec.source_files =  'MTGLDebug/iOS/*.{h,hpp,c,mm,m}'
    spec.public_header_files = 
      'MTGLDebug/iOS/MTGLDebug.h',
      'MTGLDebug/iOS/MTGLDebugImpl.h',
      'MTGLDebug/iOS/MTGLDebugObject.h',
      'MTGLDebug/iOS/MTGLDebugObject+QuickLook.h'

    spec.dependency 'MTGLDebug/MRC'

    spec.ios.frameworks = 'Foundation', 'UIKit'
    spec.requires_arc = true
  end 

  s.subspec 'MRC' do |spec|
    spec.public_header_files = 
      'MTGLDebug/Core/MTGLDebug_GL.h',
      'MTGLDebug/Core/MTGLDebug_Platform.h'

    spec.source_files = 'MTGLDebug/Core/*.{hpp,cpp,h,mm,c,m}'

    spec.ios.frameworks = 'OpenGLES', 'CoreVideo', 'AVFoundation', 'CoreMedia', 'AudioToolbox', "CoreGraphics"
    spec.requires_arc = false

    spec.libraries = 'c++'
    spec.xcconfig = {
      'CLANG_CXX_LANGUAGE_STANDARD' => 'c++11',
      'CLANG_CXX_LIBRARY' => 'libc++'
    }
  end
  
  s.dependency 'fishhook', '~> 0.2'

end
