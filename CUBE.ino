#include <SPI.h>

#define XAXIS 0
#define YAXIS 1
#define ZAXIS 2

#define POS_X 0
#define NEG_X 1
#define POS_Z 2
#define NEG_Z 3
#define POS_Y 4
#define NEG_Y 5



#define CLEAR 10
#define RAIN 0
#define PLANE_BOING 1
#define SEND_VOXELS 2
#define WOOP_WOOP 3
#define CUBE_JUMP 4
#define GLOW 5
#define TEXT 6
#define LIT 7
#define WALKING_CUBE 8
#define WIVES 9


#define RAIN_TIME 200
#define PLANE_BOING_TIME 220
#define SEND_VOXELS_TIME 140
#define WOOP_WOOP_TIME 350
#define CUBE_JUMP_TIME 200
#define GLOW_TIME 8
#define TEXT_TIME 300
#define CLOCK_TIME 500
#define WALKING_TIME 100

uint8_t characters[10][8] = {
  {0x3C, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x3C}, //0
  {0x7E, 0x10, 0x10, 0x10, 0x10, 0x50, 0x30, 0x10}, //1
  {0x7E, 0x20, 0x10, 0x08, 0x04, 0x02, 0x42, 0x3C}, //2
  {0x3C, 0x02, 0x02, 0x02, 0x3C, 0x02, 0x02, 0x3C}, //3
  {0x02, 0x02, 0x02, 0x3E, 0x22, 0x22, 0x22, 0x22}, //4
  {0x3E, 0x01, 0x01, 0x01, 0x3E, 0x10, 0x10, 0x3E}, //5
  {0x3E, 0x41, 0x41, 0x41, 0x7E, 0x40, 0x40, 0x3E}, //6
  {0x00, 0x40, 0x20, 0x10, 0x08, 0x04, 0x7E, 0x00}, //7
  {0x3C, 0x42, 0x42, 0x42, 0x3C, 0x42, 0x42, 0x3C}, //8
  {0x3C, 0x02, 0x02, 0x02, 0x3E, 0x42, 0x42, 0x3C}, //9
};

uint8_t cube[8][8];
uint8_t currentEffect;

uint16_t timer;
uint16_t modeTimer;

uint64_t randomTimer;
int16_t coord[3];
int8_t vector[3];
int8_t pos;

bool loading;
char command;
void setup() {
Serial.begin(9600);
  loading = true;
  randomTimer = 0;
  currentEffect =10;

  SPI.begin();
  SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));



  
}

void loop() {
  
  randomTimer++;
  
   
   if(Serial.available() > 0){
    command = Serial.read();
    switch(command){
      case 'R': currentEffect=0; break;
      case 'P': currentEffect=1; break;
      case 'S': currentEffect=8; break;
      case 'W': currentEffect=3; break;
      case 'C': currentEffect=4; break;
      case 'G': currentEffect=9; break;
      case 'T': currentEffect=6; break;
      case 'L': currentEffect=7; break;
      case 'Z': modeTimer -=40; changeMode(); break;
      case 'X': modeTimer +=40; changeMode(); break;
    }}
  

  switch (currentEffect) {
    case 0: rain(); break;
    case 1: planeBoing(); break;
    case 2: sendVoxels(); break;
    case 3: woopWoop(); break;
    case 4: cubeJump(); break;
    case 5: glow(); break;
    case 6: text("0123456789", 10); break;
    case 7: lit(); break;
    case 8: walkingCube(); break;
    case 9: sinusFill(); break;
    case 10: clearCube(); break;
    
   
   
    
  }
  renderCube();
}
void changeMode() {
  
  clearCube();
  loading = true;
  timer = 0;
  
switch (currentEffect) {
    case RAIN: timer = RAIN_TIME; break;
    case PLANE_BOING: timer = PLANE_BOING_TIME; break;
    case 2: timer = SEND_VOXELS_TIME; break;
    case 3: timer = WOOP_WOOP_TIME; break;
    case 4: timer = CUBE_JUMP_TIME; break;
    case 5: timer = GLOW_TIME; break;
    case 6: timer = TEXT_TIME; break;
    case 7: timer = CLOCK_TIME; break;
    case 8: timer = WALKING_CUBE; break;
}}


void renderCube() {
  for (uint8_t i = 0; i < 8; i++) {
    digitalWrite(SS, LOW);
    SPI.transfer(0x01 << i);
    for (uint8_t j = 0; j < 8; j++) {
      SPI.transfer(cube[i][j]);
    }
    digitalWrite(SS, HIGH);
  }
}
void sinusFill() {
  if (loading) {
    clearCube();
    loading = false;
  }
  timer++;
  if (timer > 50) {
    timer = 0;
    clearCube();
    if (++pos > 10) pos = 0;
    for (uint8_t i = 0; i < 8; i++) {
      for (uint8_t j = 0; j < 8; j++) {
        int8_t sinZ = 4 + ((float)sin((float)(i + pos) / 2) * 2);
        for (uint8_t y = 0; y < sinZ; y++) {
          setVoxel(i, y, j);
        }
      }
    }
  }
}


void walkingCube() {
  if (loading) {
    clearCube();
    loading = false;
    for (byte i = 0; i < 3; i++) {
      
      coord[i] = 300;
      vector[i] = random(3, 8) * 15;
    }
  }
  timer++;
  if (timer > modeTimer) {
    timer = 0;
    clearCube();
    for (byte i = 0; i < 3; i++) {
      coord[i] += vector[i];
      if (coord[i] < 1) {
        coord[i] = 1;
        vector[i] = -vector[i];
        vector[i] += random(0, 6) - 3;
      }
      if (coord[i] > 700 - 100) {
        coord[i] = 700 - 100;
        vector[i] = -vector[i];
        vector[i] += random(0, 6) - 3;
      }
    }

    int8_t thisX = coord[0] / 100;
    int8_t thisY = coord[1] / 100;
    int8_t thisZ = coord[2] / 100;

    setVoxel(thisX, thisY, thisZ);
    setVoxel(thisX + 1, thisY, thisZ);
    setVoxel(thisX, thisY + 1, thisZ);
    setVoxel(thisX, thisY, thisZ + 1);
    setVoxel(thisX + 1, thisY + 1, thisZ);
    setVoxel(thisX, thisY + 1, thisZ + 1);
    setVoxel(thisX + 1, thisY, thisZ + 1);
    setVoxel(thisX + 1, thisY + 1, thisZ + 1);
  }
}

void rain() {
  if (loading) {
    clearCube();
    loading = false;
  }
  timer++;
  if (timer > modeTimer) {
    timer = 0;
    shift(NEG_Y);
    uint8_t numDrops = random(0, 5);
    for (uint8_t i = 0; i < numDrops; i++) {
      setVoxel(random(0, 8), 7, random(0, 8));
    }
  }
}

uint8_t planePosition = 0;
uint8_t planeDirection = 0;
bool looped = false;

void planeBoing() {
  if (loading) {
    clearCube();
    uint8_t axis = random(0, 3);
    planePosition = random(0, 2) * 7;
    setPlane(axis, planePosition);
    if (axis == XAXIS) {
      if (planePosition == 0) {
        planeDirection = POS_X;
      } else {
        planeDirection = NEG_X;
      }
    } else if (axis == YAXIS) {
      if (planePosition == 0) {
        planeDirection = POS_Y;
      } else {
        planeDirection = NEG_Y;
      }
    } else if (axis == ZAXIS) {
      if (planePosition == 0) {
        planeDirection = POS_Z;
      } else {
        planeDirection = NEG_Z;
      }
    }
    timer = 0;
    looped = false;
    loading = false;
  }

  timer++;
  if (timer > modeTimer) {
    timer = 0;
    shift(planeDirection);
    if (planeDirection % 2 == 0) {
      planePosition++;
      if (planePosition == 7) {
        if (looped) {
          loading = true;
        } else {
          planeDirection++;
          looped = true;
        }
      }
    } else {
      planePosition--;
      if (planePosition == 0) {
        if (looped) {
          loading = true;
        } else {
          planeDirection--;
          looped = true;
        }
      }
    }
  }
}

uint8_t selX = 0;
uint8_t selY = 0;
uint8_t selZ = 0;
uint8_t sendDirection = 0;
bool sending = false;

void sendVoxels() {
  if (loading) {
    clearCube();
    for (uint8_t x = 0; x < 8; x++) {
      for (uint8_t z = 0; z < 8; z++) {
        setVoxel(x, random(0, 2) * 7, z);
      }
    }
    loading = false;
  }

  timer++;
  if (timer > modeTimer) {
    timer = 0;
    if (!sending) {
      selX = random(0, 8);
      selZ = random(0, 8);
      if (getVoxel(selX, 0, selZ)) {
        selY = 0;
        sendDirection = POS_Y;
      } else if (getVoxel(selX, 7, selZ)) {
        selY = 7;
        sendDirection = NEG_Y;
      }
      sending = true;
    } else {
      if (sendDirection == POS_Y) {
        selY++;
        setVoxel(selX, selY, selZ);
        clearVoxel(selX, selY - 1, selZ);
        if (selY == 7) {
          sending = false;
        }
      } else {
        selY--;
        setVoxel(selX, selY, selZ);
        clearVoxel(selX, selY + 1, selZ);
        if (selY == 0) {
          sending = false;
        }
      }
    }
  }
}

uint8_t cubeSize = 0;
bool cubeExpanding = true;

void woopWoop() {
  if (loading) {
    clearCube();
    cubeSize = 2;
    cubeExpanding = true;
    loading = false;
  }

  timer++;
  if (timer > modeTimer) {
    timer = 0;
    if (cubeExpanding) {
      cubeSize += 2;
      if (cubeSize == 8) {
        cubeExpanding = false;
      }
    } else {
      cubeSize -= 2;
      if (cubeSize == 2) {
        cubeExpanding = true;
      }
    }
    clearCube();
    drawCube(4 - cubeSize / 2, 4 - cubeSize / 2, 4 - cubeSize / 2, cubeSize);
  }
}

uint8_t xPos;
uint8_t yPos;
uint8_t zPos;

void cubeJump() {
  if (loading) {
    clearCube();
    xPos = random(0, 2) * 7;
    yPos = random(0, 2) * 7;
    zPos = random(0, 2) * 7;
    cubeSize = 8;
    cubeExpanding = false;
    loading = false;
  }

  timer++;
  if (timer > modeTimer) {
    timer = 0;
    clearCube();
    if (xPos == 0 && yPos == 0 && zPos == 0) {
      drawCube(xPos, yPos, zPos, cubeSize);
    } else if (xPos == 7 && yPos == 7 && zPos == 7) {
      drawCube(xPos + 1 - cubeSize, yPos + 1 - cubeSize, zPos + 1 - cubeSize, cubeSize);
    } else if (xPos == 7 && yPos == 0 && zPos == 0) {
      drawCube(xPos + 1 - cubeSize, yPos, zPos, cubeSize);
    } else if (xPos == 0 && yPos == 7 && zPos == 0) {
      drawCube(xPos, yPos + 1 - cubeSize, zPos, cubeSize);
    } else if (xPos == 0 && yPos == 0 && zPos == 7) {
      drawCube(xPos, yPos, zPos + 1 - cubeSize, cubeSize);
    } else if (xPos == 7 && yPos == 7 && zPos == 0) {
      drawCube(xPos + 1 - cubeSize, yPos + 1 - cubeSize, zPos, cubeSize);
    } else if (xPos == 0 && yPos == 7 && zPos == 7) {
      drawCube(xPos, yPos + 1 - cubeSize, zPos + 1 - cubeSize, cubeSize);
    } else if (xPos == 7 && yPos == 0 && zPos == 7) {
      drawCube(xPos + 1 - cubeSize, yPos, zPos + 1 - cubeSize, cubeSize);
    }
    if (cubeExpanding) {
      cubeSize++;
      if (cubeSize == 8) {
        cubeExpanding = false;
        xPos = random(0, 2) * 7;
        yPos = random(0, 2) * 7;
        zPos = random(0, 2) * 7;
      }
    } else {
      cubeSize--;
      if (cubeSize == 1) {
        cubeExpanding = true;
      }
    }
  }
}

bool glowing;
uint16_t glowCount = 0;

void glow() {
  if (loading) {
    clearCube();
    glowCount = 0;
    glowing = true;
    loading = false;
  }

  timer++;
  if (timer > modeTimer) {
    timer = 0;
    if (glowing) {
      if (glowCount < 448) {
        do {
          selX = random(0, 8);
          selY = random(0, 8);
          selZ = random(0, 8);
        } while (getVoxel(selX, selY, selZ));
        setVoxel(selX, selY, selZ);
        glowCount++;
      } else if (glowCount < 512) {
        lightCube();
        glowCount++;
      } else {
        glowing = false;
        glowCount = 0;
      }
    } else {
      if (glowCount < 448) {
        do {
          selX = random(0, 8);
          selY = random(0, 8);
          selZ = random(0, 8);
        } while (!getVoxel(selX, selY, selZ));
        clearVoxel(selX, selY, selZ);
        glowCount++;
      } else {
        clearCube();
        glowing = true;
        glowCount = 0;
      }
    }
  }
}

uint8_t charCounter = 0;
uint8_t charPosition = 0;

void text(char string[], uint8_t len) {
  if (loading) {
    clearCube();
    charPosition = -1;
    charCounter = 0;
    loading = false;
  }
  timer++;
  if (timer > modeTimer) {
    timer = 0;

    shift(NEG_Z);
    charPosition++;

    if (charPosition == 7) {
      charCounter++;
      if (charCounter > len - 1) {
        charCounter = 0;
      }
      charPosition = 0;
    }

    if (charPosition == 0) {
      for (uint8_t i = 0; i < 8; i++) {
        cube[i][0] = characters[string[charCounter] - '0'][i];
      }
    }
  }
}

void lit() {
  
  if (loading) {
    clearCube();
    loading = false;
  }
    timer ++;
    if (timer > modeTimer) {
 
       for (uint8_t i = 0; i < 8; i++) {
    for (uint8_t j = 0; j < 8; j++) {
      cube[i][j] = 0xFF;
    }
  }
}}

     
       
      
    
    
  


void setVoxel(uint8_t x, uint8_t y, uint8_t z) {
  cube[7 - y][7 - z] |= (0x01 << x);
}

void clearVoxel(uint8_t x, uint8_t y, uint8_t z) {
  cube[7 - y][7 - z] ^= (0x01 << x);
}

bool getVoxel(uint8_t x, uint8_t y, uint8_t z) {
  return (cube[7 - y][7 - z] & (0x01 << x)) == (0x01 << x);
}

void setPlane(uint8_t axis, uint8_t i) {
  for (uint8_t j = 0; j < 8; j++) {
    for (uint8_t k = 0; k < 8; k++) {
      if (axis == XAXIS) {
        setVoxel(i, j, k);
      } else if (axis == YAXIS) {
        setVoxel(j, i, k);
      } else if (axis == ZAXIS) {
        setVoxel(j, k, i);
      }
    }
  }
}

void shift(uint8_t dir) {

  if (dir == POS_X) {
    for (uint8_t y = 0; y < 8; y++) {
      for (uint8_t z = 0; z < 8; z++) {
        cube[y][z] = cube[y][z] << 1;
      }
    }
  } else if (dir == NEG_X) {
    for (uint8_t y = 0; y < 8; y++) {
      for (uint8_t z = 0; z < 8; z++) {
        cube[y][z] = cube[y][z] >> 1;
      }
    }
  } else if (dir == POS_Y) {
    for (uint8_t y = 1; y < 8; y++) {
      for (uint8_t z = 0; z < 8; z++) {
        cube[y - 1][z] = cube[y][z];
      }
    }
    for (uint8_t i = 0; i < 8; i++) {
      cube[7][i] = 0;
    }
  } else if (dir == NEG_Y) {
    for (uint8_t y = 7; y > 0; y--) {
      for (uint8_t z = 0; z < 8; z++) {
        cube[y][z] = cube[y - 1][z];
      }
    }
    for (uint8_t i = 0; i < 8; i++) {
      cube[0][i] = 0;
    }
  } else if (dir == POS_Z) {
    for (uint8_t y = 0; y < 8; y++) {
      for (uint8_t z = 1; z < 8; z++) {
        cube[y][z - 1] = cube[y][z];
      }
    }
    for (uint8_t i = 0; i < 8; i++) {
      cube[i][7] = 0;
    }
  } else if (dir == NEG_Z) {
    for (uint8_t y = 0; y < 8; y++) {
      for (uint8_t z = 7; z > 0; z--) {
        cube[y][z] = cube[y][z - 1];
      }
    }
    for (uint8_t i = 0; i < 8; i++) {
      cube[i][0] = 0;
    }
  }
}

void drawCube(uint8_t x, uint8_t y, uint8_t z, uint8_t s) {
  for (uint8_t i = 0; i < s; i++) {
    setVoxel(x, y + i, z);
    setVoxel(x + i, y, z);
    setVoxel(x, y, z + i);
    setVoxel(x + s - 1, y + i, z + s - 1);
    setVoxel(x + i, y + s - 1, z + s - 1);
    setVoxel(x + s - 1, y + s - 1, z + i);
    setVoxel(x + s - 1, y + i, z);
    setVoxel(x, y + i, z + s - 1);
    setVoxel(x + i, y + s - 1, z);
    setVoxel(x + i, y, z + s - 1);
    setVoxel(x + s - 1, y, z + i);
    setVoxel(x, y + s - 1, z + i);
  }
}

void lightCube() {
 
       for (uint8_t i = 0; i < 8; i++) {
    for (uint8_t j = 0; j < 8; j++) {
      cube[i][j] = 0xFF;
    }
  }
}
    
  


void clearCube() {
  for (uint8_t i = 0; i < 8; i++) {
    for (uint8_t j = 0; j < 8; j++) {
      cube[i][j] = 0;
    }
  }
}
