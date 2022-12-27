/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ecamara <ecamara@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/23 16:40:10 by eperaita          #+#    #+#             */
/*   Updated: 2022/12/27 17:38:46 by ecamara          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../sim.h"

void	set_map(t_data *data)
{
	data->map.img = mlx_new_image(data->mlx.mlx, WIDTH, HEIGHT);
	data->map.add = (int *)mlx_get_data_addr(data->map.img,
			&data->map.bpp, &data->map.w,
			&data->map.endian);
}

void	init_mlx(t_data *data)
{
	data->mlx.mlx = mlx_init();
	data->mlx.win = mlx_new_window(data->mlx.mlx, WIDTH, HEIGHT, "SIM");
}

int	random_number(int max)
{
	return (rand() % max);
}

void	printf_uv(t_uv uv)
{
	printf("[%lf][%lf]\n", uv.x, uv.y);
}

void proyect(t_data *data)
{
	int x;
	int y;

	y = 0;
	while (y < HEIGHT)
	{
		x = 0;
		while (x < WIDTH)
		{
			double value = data->cSystem[coord(x / 10, y / 10)].density;
			data->bakery.frames[data->bakery.index].add[y * WIDTH + x] = convert_rgb(col_to_255(vector(value,value, value)));
			x++;
		}
		y++;
	}
}

int	showframes(t_data *data)
{
	/*if (BAKED)
	{
		data->bakery.index++;
		if (data->bakery.index == FRAMES)
			data->bakery.backed = 1;
		if (!data->bakery.backed && data->bakery.index < FRAMES)
		{
			system("clear");
			printf("[%lf]\n", data->bakery.index / (double)FRAMES);
			render(data);
		//	mlx_put_image_to_window(data->mlx.mlx, data->mlx.win, data->bakery.frames[data->bakery.index].img,0,0);
		}
		else
		{
			if (data->time)
			{
				gettimeofday(&data->later, NULL);
				printf("[%lf]\n", ft_to_sec(data->later) - ft_to_sec(data->now));
				data->time = 0;
			}
			if (data->bakery.index >= FRAMES)
				data->bakery.index = 0;
			if (data->framerate.fast)
				data->framerate.usleep += 1000;
			if (data->framerate.slow)
				data->framerate.usleep -= 1000;
			mlx_put_image_to_window(data->mlx.mlx, data->mlx.win, data->bakery.frames[data->bakery.index].img,0,0);
			usleep(data->framerate.usleep);
			data->bakery.index++;
		}
	}
	else*/
	{
		diffusion(data);
		advection(data);
		proyect(data);
		mlx_put_image_to_window(data->mlx.mlx, data->mlx.win, data->bakery.frames->img, 0,0);
		/*advection(data);
		proyect(data);
		mlx_put_image_to_window(data->mlx.mlx, data->mlx.win, data->bakery.frames->img, 0,0);*/
		//exit(0);
		//clearMulti(data);
	}
	return (0);
}

void	create_threat(t_data *data)
{
	data->mutex = (pthread_t *)malloc(sizeof(pthread_t) * THREADS);
}

int	insideRect(t_uv point, t_uv rightTop, t_uv botLeft)
{
	if (point.x < rightTop.x && point.x > botLeft.x && point.y < botLeft.y && point.y > rightTop.y)
		return(1);
	return (0);
}

int	intclamp(int min, int max, int value)
{
	if (value < min)
		return min;
	if (value > max)
		return max;
	return value;
}

/*
void	setParticules(t_data *data, char *argv)
{
	int	fd;
	char	*line;
	int		len;
	int		i;

	len = 0;
	fd = open(argv, O_RDONLY);
	if (fd < 0)
	{
		printf("Input file error\n");
		exit(0);
	}
	do
	{
		line = get_next_line(fd);
		if (line == NULL || line[0] == '\n'|| line[0] == '\0')
			break ;
		len++;
		free(line);
	} while (1);
	data->particules.array = malloc(sizeof(t_particule) * (len + 1));
	if (!data->particules.array)
		exit(0);
	data->particules.max = len;
	fd = open(argv, O_RDONLY);
	i = 0;
	do
	{
		line = get_next_line(fd);
		if (line == NULL || line[0] == '\n' || line[0] == '\0')
			break ;
		data->particules.array[i].id = i;
		data->particules.array[i].pos = getUv(line);
		data->particules.array[i].u = uv(0,0);
		free(line);
		i++;
	} while (1);
}*/

void	drawRectangule(t_data *data, int minX, int maxX, int minY, int maxY, int col)
{
	int	i;
	int	j;

	j = minY;
	while(j < maxY)
	{
		i = minX;
		while (i < maxX)
		{
			data->map.add[j * WIDTH + i] = col;
			i++;
		}
		j++;
	}
}

void	backeryStart(t_data *data)
{
	int	i;
	
	i = 0;
	data->bakery.index = 0;
	data->bakery.backed = 0;
	if (BAKED)
	{
		while (i < FRAMES)
		{
			data->bakery.frames[i].img = mlx_new_image(data->mlx.mlx, WIDTH, HEIGHT);
			data->bakery.frames[i].add = (int *)mlx_get_data_addr(data->bakery.frames[i].img,
				&data->bakery.frames[i].bpp, &data->bakery.frames[i].w,
				&data->bakery.frames[i].endian);
			i++;
		}
	}
	else
	{
		data->bakery.frames[i].img = mlx_new_image(data->mlx.mlx, WIDTH, HEIGHT);
			data->bakery.frames[i].add = (int *)mlx_get_data_addr(data->bakery.frames[i].img,
				&data->bakery.frames[i].bpp, &data->bakery.frames[i].w,
				&data->bakery.frames[i].endian);
	}
}

int	ft_eventspress(int keycode, t_data *data)
{
	if (keycode == 125)
		data->framerate.slow = 1;
	else if (keycode == 126)
		data->framerate.fast = 1;
	else if (keycode == 49)
		data->bakery.index = 0;
	return (0);
}

int	ft_eventsrealese(int keycode, t_data *data)
{
	if (keycode == 125)
		data->framerate.slow = 0;
	else if (keycode == 126)
		data->framerate.fast = 0;
	return (0);
}

void initData(t_data *data)
{
	int	x;
	int y;

	y = 0;
	while (y < SHEIGHT)
	{
		x = 0;
		while (x < SWIDTH)
		{
			data->cSystem[coord(x, y)].density = 0;
			data->nSystem[coord(x, y)].density = 0;
			if (random_number(5) == 0)
				data->cSystem[coord(x, y)].density = 1;
			data->cSystem[coord(x, y)].u = uv(0,0);
			data->nSystem[coord(x, y)].u = uv(0,0);
			if (x == 50 && y == 50)
			{
				data->cSystem[coord(x,y)].u = uv(0, 1);
				data->cSystem[coord(x, y)].density = 1;
			}
			x++;
		}
		y++;
	}
	data->k = 0.1;
}

int	main(int argc, char *argv[])
{
	t_data	data;

	init_mlx(&data);
	create_threat(&data);
	set_map(&data);
	backeryStart(&data);
	gettimeofday(&data.now, NULL);
	data.time = 1;
	(void)argv;
	(void)argc;
	initData(&data);
	mlx_loop_hook(data.mlx.mlx, showframes, &data);
	mlx_hook(data.mlx.win, 2, 1L << 0, ft_eventspress, &data);
	mlx_hook(data.mlx.win, 3, 1L << 0, ft_eventsrealese, &data);
	mlx_loop(data.mlx.mlx);
	return (0);
}
