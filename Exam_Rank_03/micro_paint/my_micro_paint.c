#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef struct s_zone
{
	int		width;
	int		height;
	char	background;
}	t_zone;

typedef struct s_shape
{
	char	type;
	float	width;
	float	height;
	float	x;
	float	y;
	char	draw;
}	t_shape;

int	ft_strlen(char *str)
{
	int	i;

	i = 0;
	while (str[i])
		i++;
	return (i);
}

int	err_msg(char *str, int err)
{
	write(1, str, ft_strlen(str));
	return (err);
}

int	clear_all(FILE *fd, char *drawing)
{
	fclose(fd);
	if (drawing)
		free(drawing);
	return (1);
}

void	print_all(char *drawing, t_zone *zone)
{
	int	i;

	i = 0;
	while (i < zone->height)
	{
		write(1, drawing + (i * zone->width), zone->width);
		write(1, "\n", 1);
		i++;
	}
}

int	check_zone(t_zone *zone)
{
	return (zone->width > 0 && zone->width <= 300 && zone->height > 0 && zone->height <= 300);
}

int	get_zone(t_zone *zone, FILE *fd)
{
	int	scan_ret;

	scan_ret = fscanf(fd, "%d %d %c\n", &zone->width, &zone->height, &zone->background);
	if (scan_ret != 3)
		return (0);
	if (!check_zone(zone))
		return (0);
	if (scan_ret == -1)
		return (0);
	return (1);
}

char	*fill_background(t_zone *zone)
{
	char	*drawing;
	int		i;

	drawing = malloc(sizeof(char *) * (zone->width * zone->height));
	i = 0;
	while (i < zone->width * zone->height)
		drawing[i++] = zone->background;
	return (drawing);
}

int	check_shape(t_shape *shape)
{
	return ((shape->type == 'R' || shape->type == 'r') && shape->width > 0.00000000 && shape->height > 0.00000000);
}

int	in_rectangle(float x, float y, t_shape *shape)
{
	if (((x < shape->x || (shape->x + shape->width < x)) || (y < shape->y)) || (shape->y + shape->height < y))
		return (0);
	if (((x - shape->x < 1.00000000) || ((shape->x + shape->width) - x < 1.00000000)) || ((y - shape->y < 1.00000000 || ((shape->y + shape->height) - y < 1.00000000))))
		return (2);
	return (1);
}

void	draw_shape(char **drawing, t_shape *shape, t_zone *zone)
{
	int	i;
	int	j;
	int	ret;

	i = 0;
	while (i < zone->height)
	{
		j = 0;
		while (j < zone->width)
		{
			ret = in_rectangle((float)j, (float)i, shape);
			if ((shape->type == 'r' && ret == 2) || (shape->type == 'R' && ret))
				(*drawing)[(i * zone->width) + j] = shape->draw;
			j++;
		}
		i++;
	}
}

int	draw_shapes(FILE *fd, char **drawing, t_zone *zone)
{
	t_shape	tmp;
	int		scan_ret;

	while ((scan_ret = fscanf(fd, "%c %f %f %f %f %c\n", &tmp.type, &tmp.x, &tmp.y, &tmp.width, &tmp.height, &tmp.draw)) == 6)
	{
		if (!check_shape(&tmp))
			return (0);
		draw_shape(drawing, &tmp, zone);
	}
	if (scan_ret != -1)
		return (0);
	return (1);
}

int	main(int argc, char *argv[])
{
	FILE	*file;
	char	*drawing;
	t_zone	zone;

	zone.width = 0;
	zone.height = 0;
	zone.background = 0;
	if (argc != 2)
		return (err_msg("Error: argument\n", 1));
	if (!(file = fopen(argv[1], "r")))
		return (err_msg("Error: Operation file corrupted\n", 1));
	if (!get_zone(&zone, file))
		return (clear_all(file, 0) && err_msg("Error: Operation file corrupted\n", 1));
	if (!(drawing = fill_background(&zone)))
		return (clear_all(file, 0) && err_msg("Error: Operation file corrupted\n", 1));
	if (!draw_shapes(file, &drawing, &zone))
		return (clear_all(file, drawing) && err_msg("Error: Operation file corrupted\n", 1));
	print_all(drawing, &zone);
	clear_all(file, drawing);
	return (0);
}