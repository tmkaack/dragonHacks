  int width = 80;
  int height = 40;
  int calcNumber = 1;
  tft.fillScreen(BLACK);  
  tft.setCursor(10, 70);
  tft.setTextColor(WHITE); tft.setTextSize(3);
  tft.print("Carbs: ");
  tft.fillRect(118, 60, 100, 40, GRAY);
  drawBackButton();
  for(int i = 0; i < 4; i++){
    for(int j = 0; j < 3; j++){
      tft.fillRect(10+j*width, 120+i*height, width-2, height-2, GRAY);
      tft.setCursor(10+j*width+30, 120+i*height+8);
      tft.setTextColor(WHITE); tft.setTextSize(3);
      tft.print(calcNumber);
      calcNumber++;
    }
  }


// click detection
int xb, yb, level = 0, button;
int buttons[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, -1, 0, -2};
if(p.y < maxY && p.y > minY && p.x < maxX && p.x > minX){
	yb = p.y%width;
	xb = p.x%height;
	level = buttons[yb + xb*3];
	if (level == -1){
		//currentLevel needs to be tracked
		currentLevel += button;
	}else if(level == -2){
		currentLevel = bottun;
	}else{
		level *=10;
		button += level;
	}
} 