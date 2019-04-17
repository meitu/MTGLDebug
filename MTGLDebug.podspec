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

  s.authors      = { 'zxj' => 'zxj@meitu.com' , 'wzq' => 'wzq@meitu.com' }
  s.platform     = :ios, '8.0'
  s.source       = { :git => "git@github.com:meitu/MTGLDebug.git", :tag => "#{s.version}" }
  

  s.subspec 'ARC' do |spec|
    spec.ios.frameworks = 'OpenGLES', 'CoreVideo', 'AVFoundation', 'CoreMedia', 'AudioToolbox'
    spec.source_files =  'MTGLDebug/iOS/*.{h,hpp,c,mm,m}'
    spec.public_header_files = 'MTGLDebug/iOS/*.{h,hpp}'
    spec.libraries = 'c++'
    spec.requires_arc = true
    spec.dependency 'MTGLDebug/MRC'
  end 

  s.subspec 'MRC' do |spec|
    spec.ios.frameworks = 'OpenGLES', 'CoreVideo', 'AVFoundation', 'CoreMedia', 'AudioToolbox'
    spec.libraries = 'c++'
    spec.requires_arc = false
    spec.source_files = 'MTGLDebug/Core/*.{hpp,cpp,h,mm,c,m}'
    spec.public_header_files = 'MTGLDebug/Core/*.{h,hpp}'
  end

  s.frameworks = 'CoreFoundation', 'CoreVideo', 'OpenGLES', 'UIKit', 'Foundation'             
  
  s.library = 'c++'

  s.dependency 'fishhook', '~> 0.2'

end
