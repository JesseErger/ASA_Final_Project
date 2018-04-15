#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "obj_parser.h"
#include "list.h"
#include "string_extra.h"

#define WHITESPACE " \t\n\r"

void obj_free_half_list(list *listo)
{
	list_delete_all(listo);
	free(listo->names);
}

int obj_convert_to_list_index(int current_max, int index)
{
	if(index == 0)  //no index
		return -1;
		
	if(index < 0)  //relative to current list position
		return current_max + index;
		
	return index - 1;  //normal counting index
}

void obj_convert_to_list_index_v(int current_max, int *indices)
{
	for(int i=0; i<MAX_VERTEX_COUNT; i++)
		indices[i] = obj_convert_to_list_index(current_max, indices[i]);
}

void obj_set_material_defaults(obj_material *mtl)
{
	mtl->amb[0] = 0.2;
	mtl->amb[1] = 0.2;
	mtl->amb[2] = 0.2;
	mtl->diff[0] = 0.8;
	mtl->diff[1] = 0.8;
	mtl->diff[2] = 0.8;
	mtl->spec[0] = 1.0;
	mtl->spec[1] = 1.0;
	mtl->spec[2] = 1.0;
	mtl->reflect = -1.0;
	mtl->trans = 1;
	mtl->glossy = 98;
	mtl->shiny = 0;
	mtl->refract_index = -1.0;
	mtl->texture_filename[0] = '\0';
}

int obj_parse_vertex_index(int *vertex_index, int *texture_index, int *normal_index)
{
	char *temp_str;
	char *token;
	int vertex_count = 0;

	
	while( (token = strtok(NULL, WHITESPACE)) != NULL)
	{
		if(texture_index != NULL)
			texture_index[vertex_count] = 0;
		if(normal_index != NULL)
		normal_index[vertex_count] = 0;

		vertex_index[vertex_count] = atoi( token );
		
		if(contains(token, "//"))  //normal only
		{
			temp_str = strchr(token, '/');
			temp_str++;
			normal_index[vertex_count] = atoi( ++temp_str );
		}
		else if(contains(token, "/"))
		{
			temp_str = strchr(token, '/');
			texture_index[vertex_count] = atoi( ++temp_str );

			if(contains(temp_str, "/"))
			{
				temp_str = strchr(temp_str, '/');
				normal_index[vertex_count] = atoi( ++temp_str );
			}
		}
		
		vertex_count++;
	}

	return vertex_count;
}

obj_face* obj_parse_face(obj_growable_scene_data *scene)
{
	int vertex_count;
	obj_face *face = (obj_face*)malloc(sizeof(obj_face));
	
	vertex_count = obj_parse_vertex_index(face->vertex_index, face->texture_index, face->normal_index);
	obj_convert_to_list_index_v(scene->vertex_list.item_count, face->vertex_index);
	obj_convert_to_list_index_v(scene->vertex_texture_list.item_count, face->texture_index);
	obj_convert_to_list_index_v(scene->vertex_normal_list.item_count, face->normal_index);
	face->vertex_count = vertex_count;

	return face;
}

obj_sphere* obj_parse_sphere(obj_growable_scene_data *scene)
{
	int temp_indices[MAX_VERTEX_COUNT];

	obj_sphere *obj = (obj_sphere*)malloc(sizeof(obj_sphere));
	obj_parse_vertex_index(temp_indices, obj->texture_index, NULL);
	obj_convert_to_list_index_v(scene->vertex_texture_list.item_count, obj->texture_index);
	obj->pos_index = obj_convert_to_list_index(scene->vertex_list.item_count, temp_indices[0]);
	obj->up_normal_index = obj_convert_to_list_index(scene->vertex_normal_list.item_count, temp_indices[1]);
	obj->equator_normal_index = obj_convert_to_list_index(scene->vertex_normal_list.item_count, temp_indices[2]);

	return obj;
}

obj_plane* obj_parse_plane(obj_growable_scene_data *scene)
{
	int temp_indices[MAX_VERTEX_COUNT];

	obj_plane *obj = (obj_plane*)malloc(sizeof(obj_plane));
	obj_parse_vertex_index(temp_indices, obj->texture_index, NULL);
	obj_convert_to_list_index_v(scene->vertex_texture_list.item_count, obj->texture_index);
	obj->pos_index = obj_convert_to_list_index(scene->vertex_list.item_count, temp_indices[0]);
	obj->normal_index = obj_convert_to_list_index(scene->vertex_normal_list.item_count, temp_indices[1]);
	obj->rotation_normal_index = obj_convert_to_list_index(scene->vertex_normal_list.item_count, temp_indices[2]);

	return obj;
}

obj_light_point* obj_parse_light_point(obj_growable_scene_data *scene)
{
	obj_light_point *o= (obj_light_point*)malloc(sizeof(obj_light_point));
	o->pos_index = obj_convert_to_list_index(scene->vertex_list.item_count, atoi( strtok(NULL, WHITESPACE)) );
	return o;
}

obj_light_quad* obj_parse_light_quad(obj_growable_scene_data *scene)
{
	obj_light_quad *o = (obj_light_quad*)malloc(sizeof(obj_light_quad));
	obj_parse_vertex_index(o->vertex_index, NULL, NULL);
	obj_convert_to_list_index_v(scene->vertex_list.item_count, o->vertex_index);

	return o;
}

obj_light_disc* obj_parse_light_disc(obj_growable_scene_data *scene)
{
	int temp_indices[MAX_VERTEX_COUNT];

	obj_light_disc *obj = (obj_light_disc*)malloc(sizeof(obj_light_disc));
	obj_parse_vertex_index(temp_indices, NULL, NULL);
	obj->pos_index = obj_convert_to_list_index(scene->vertex_list.item_count, temp_indices[0]);
	obj->normal_index = obj_convert_to_list_index(scene->vertex_normal_list.item_count, temp_indices[1]);

	return obj;
}

obj_vector* obj_parse_vector()
{
	obj_vector *v = (obj_vector*)malloc(sizeof(obj_vector));
	v->e[0] = atof( strtok(NULL, WHITESPACE));
	v->e[1] = atof( strtok(NULL, WHITESPACE));
	v->e[2] = atof( strtok(NULL, WHITESPACE));
	return v;
}

obj_vector* obj_parse_2d_vector()
{
	obj_vector *v = (obj_vector*)malloc(sizeof(obj_vector));
	v->e[0] = atof( strtok(NULL, WHITESPACE));
	v->e[1] = atof( strtok(NULL, WHITESPACE));
	v->e[2] = -1.0;
	return v;
}

void obj_parse_camera(obj_growable_scene_data *scene, obj_camera *camera)
{
	int indices[3];
	obj_parse_vertex_index(indices, NULL, NULL);
	camera->camera_pos_index = obj_convert_to_list_index(scene->vertex_list.item_count, indices[0]);
	camera->camera_look_point_index = obj_convert_to_list_index(scene->vertex_list.item_count, indices[1]);
	camera->camera_up_norm_index = obj_convert_to_list_index(scene->vertex_normal_list.item_count, indices[2]);
}


void obj_init_temp_storage(obj_growable_scene_data *growable_data)
{
	list_make(&growable_data->vertex_list, 10, 1);
	list_make(&growable_data->vertex_normal_list, 10, 1);
	list_make(&growable_data->vertex_texture_list, 10, 1);
	
	list_make(&growable_data->face_list, 10, 1);
	list_make(&growable_data->sphere_list, 10, 1);
	list_make(&growable_data->plane_list, 10, 1);
	
	list_make(&growable_data->light_point_list, 10, 1);
	list_make(&growable_data->light_quad_list, 10, 1);
	list_make(&growable_data->light_disc_list, 10, 1);
	
	list_make(&growable_data->material_list, 10, 1);	
	
	growable_data->camera = NULL;
}

void obj_free_temp_storage(obj_growable_scene_data *growable_data)
{
	obj_free_half_list(&growable_data->vertex_list);
	obj_free_half_list(&growable_data->vertex_normal_list);
	obj_free_half_list(&growable_data->vertex_texture_list);
	
	obj_free_half_list(&growable_data->face_list);
	obj_free_half_list(&growable_data->sphere_list);
	obj_free_half_list(&growable_data->plane_list);
	
	obj_free_half_list(&growable_data->light_point_list);
	obj_free_half_list(&growable_data->light_quad_list);
	obj_free_half_list(&growable_data->light_disc_list);
	
	obj_free_half_list(&growable_data->material_list);
}

void obj_copy_to_out_storage(obj_scene_data *data_out, obj_growable_scene_data *growable_data)
{
	data_out->vertex_count = growable_data->vertex_list.item_count;
	data_out->vertex_normal_count = growable_data->vertex_normal_list.item_count;
	data_out->vertex_texture_count = growable_data->vertex_texture_list.item_count;

	data_out->face_count = growable_data->face_list.item_count;
	data_out->sphere_count = growable_data->sphere_list.item_count;
	data_out->plane_count = growable_data->plane_list.item_count;

	data_out->light_point_count = growable_data->light_point_list.item_count;
	data_out->light_disc_count = growable_data->light_disc_list.item_count;
	data_out->light_quad_count = growable_data->light_quad_list.item_count;

	data_out->material_count = growable_data->material_list.item_count;
	
	data_out->vertex_list = (obj_vector**)growable_data->vertex_list.items;
	data_out->vertex_normal_list = (obj_vector**)growable_data->vertex_normal_list.items;
	data_out->vertex_texture_list = (obj_vector**)growable_data->vertex_texture_list.items;

	data_out->face_list = (obj_face**)growable_data->face_list.items;
	data_out->sphere_list = (obj_sphere**)growable_data->sphere_list.items;
	data_out->plane_list = (obj_plane**)growable_data->plane_list.items;

	data_out->light_point_list = (obj_light_point**)growable_data->light_point_list.items;
	data_out->light_disc_list = (obj_light_disc**)growable_data->light_disc_list.items;
	data_out->light_quad_list = (obj_light_quad**)growable_data->light_quad_list.items;
	
	data_out->material_list = (obj_material**)growable_data->material_list.items;
	
	data_out->camera = growable_data->camera;
}

