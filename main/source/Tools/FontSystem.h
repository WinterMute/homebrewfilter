#ifndef FONTSYSTEM_H_
#define FONTSYSTEM_H_

bool SetupDefaultFont(const char *path);
void ClearFontData();
void SetFontScale(float Scale);
float GetFontScale();
void SetFont();
bool font_folder_exists();

#endif