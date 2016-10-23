# General
* Add support for HDR cube maps
* Add the cleanup functions to the Resource classes. Currently leaking memory i think. Maybe handle this by adding deconstructors to the xxxxData types?.
* Add checks to see if the file exsits in the Model/Material/Texture/Shader/ShaderProgram classes before trying to load
* Convert back over to using exceptions hey seem like the better option (especially for hot reloading shaders/scripts/assets). Just make sure you stil have some kind of logging and use them properly.
* Look into nanogui
* Make resource directories ignore case on prefix
* Add a frame buffer class

# Maintenance
* Move unnecessary includes into the cpp files and out of the headers
* Switch to premake5
* Add Doxygen to project
* Remove TODO's from code.
* Document code
* Document Resource. It really needs it
* Learn and write unit tests. Especially for ResourePath and ReferenceCounter

# Reference / Reading
* http://bitsquid.blogspot.com/2012/01/sensible-error-handling-part-1.html
* http://bitsquid.blogspot.com/2012/02/sensible-error-handling-part-2.html
* http://bitsquid.blogspot.com/2012/02/sensible-error-handling-part-3.html
* http://graphicrants.blogspot.com/2013/08/specular-brdf-reference.html
* http://blog.selfshadow.com/publications/s2013-shading-course/karis/s2013_pbs_epic_notes_v2.pdf