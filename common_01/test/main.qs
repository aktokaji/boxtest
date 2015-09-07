//loader.addLoadPath("E:/testbed");

    //RunFromMemory("E:\\browser.exe");
    //RunFromMemory("E:\\testbed\\tlscb.exe");
    //RunFromMemory("E:\\testbed\\browser-wk2.exe");
    //RunFromMemory("E:\\testbed\\browser486.exe");

if(1)
{
    loader.setMainProgram("E:\\testbed\\browser486.exe");
    loader.addLoadPath("C:/Qt/4.8.6/bin");
    loader.addLoadPath("C:/Qt/4.8.6/plugins");
    //loader.addDebugModule("qgif4.dll");
}
else if(true)
{
    loader.setMainProgram("E:\\testbed\\browser-wk2.exe");
    loader.addLoadPath("C:/Qt/Qt5.5.0/5.5/msvc2013/bin");
    loader.addLoadPath("C:/Qt/Qt5.5.0/5.5/msvc2013/plugins");
    loader.ignoreDll("qt5svg.dll");
}
else
{
    loader.setMainProgram("E:\\browser.exe");
}
