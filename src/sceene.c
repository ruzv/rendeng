#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sceene.h"

// reads a line from a file. the caller is responsible for freeing the memory.
// returns NULL if an error occured.
char* read_line(FILE* file) {
  char* line = calloc(256, sizeof(char));
  if (line == NULL) {
    return NULL;
  }

  size_t size = 255; // lower than allocated, to allow for null terminator
  size_t lenght = 0;

  while (1) {
    int c = fgetc(file);
    if (c == EOF) {
      break;
    }

    line[lenght] = c;
    lenght += 1;

    if (c == '\n') {
      break;
    }

    if (lenght >= size) {
      size *= 2;
      line = realloc(line, size);
      if (line == NULL) {
        return NULL;
      }
    }
  }

  line[lenght] = '\0';

  return line;
}

error* load_sceene(char* file_name, object_manager* objm) {
  printf("loading sceene: %s\n", file_name);

  FILE* sceene_file = fopen(file_name, "r");
  if (sceene_file == NULL) {
    return new_error("could not open sceene file");
  }

  char descirp_char;

  while (1) {
    char* line = read_line(sceene_file);
    if (line == NULL) {
      return new_error("failed to read line");
    }

    if (strlen(line) == 0) {
      free(line);
      break;
    }

    if (line[0] == 'l') { // add light
      float x, y, z;
      float dr, dg, db;
      float sr, sg, sb;

      sscanf(
          line,
          "%c %f %f %f %f %f %f %f %f %f",
          &descirp_char,
          &x,
          &y,
          &z,
          &dr,
          &dg,
          &db,
          &sr,
          &sg,
          &sb
      );

      add_light(objm, x, y, z, dr, dg, db, sr, sg, sb);
    } else if (line[0] == 'm') { // add material
      float dr, dg, db;
      int sc;
      float reflc, refrc, refri;

      sscanf(
          line,
          "%c %f %f %f %d %f %f %f",
          &descirp_char,
          &dr,
          &dg,
          &db,
          &sc,
          &reflc,
          &refrc,
          &refri
      );

      add_material(objm, dr, dg, db, sc, reflc, refrc, refri);
    } else if (line[0] == 'p') { // add plane
      float px, py, pz, nx, ny, nz;
      int mat;

      sscanf(
          line,
          "%c %f %f %f %f %f %f %d",
          &descirp_char,
          &px,
          &py,
          &pz,
          &nx,
          &ny,
          &nz,
          &mat
      );

      add_plane(objm, px, py, pz, nx, ny, nz, &objm->materials[mat]);
    } else if (line[0] == 's') { // add sphere
      float cx, cy, cz, radius;
      int mat;

      sscanf(
          line, "%c %f %f %f %f %d", &descirp_char, &cx, &cy, &cz, &radius, &mat
      );

      add_sphere(objm, cx, cy, cz, radius, &objm->materials[mat]);
    }

    free(line);
  }

  fclose(sceene_file);

  return NULL;
}
