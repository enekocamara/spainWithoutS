/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simulation.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ecamara <ecamara@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/27 12:30:25 by ecamara           #+#    #+#             */
/*   Updated: 2022/12/27 17:45:08 by ecamara          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../sim.h"

int coord(int x, int y)
{
    return (y * SWIDTH + x);
}

void diffuseSquare(t_data *data, int x, int y)
{
    double B = data->cSystem[coord(x + 1, y)].density;
    double C = data->cSystem[coord(x - 1, y)].density;
    double D = data->cSystem[coord(x, y + 1)].density;
    double E = data->cSystem[coord(x, y - 1)].density;
    double A = 0;
    int i = 0;
    if (B > 1 || C > 1 || D > 1 || E > 1)
        printf("[%lf][%lf][%lf][%lf][%d][%d]\n", B, C, D, E, x, y);
    while (i < 20)
    {
        if (B + C + D + E != 0)
            A = data->cSystem[coord(x, y)].density + data->k * ((B + C + D + E) / 4);
        else
            A = data->cSystem[coord(x, y)].density;
        if (A != 0)
          A /= (1 + data->k);
        B = (4 * ((A * (1 + data->k)) - data->cSystem[coord(x, y)].density) / data->k) - C - D - E;
        C = (4 * ((A * (1 + data->k)) - data->cSystem[coord(x, y)].density) / data->k) - B - D - E;
        D = (4 * ((A * (1 + data->k)) - data->cSystem[coord(x, y)].density) / data->k) - B - C - E;
        E = (4 * ((A * (1 + data->k)) - data->cSystem[coord(x, y)].density) / data->k) - B - C - D;
        i++;
    }
    data->nSystem[coord(x, y)].density = A;
}

void diffusion(t_data *data)
{
    int y;
    int x;

    y = 1;
    while (y < SHEIGHT - 1)
    {
        x = 1;
        while (x < SWIDTH - 1)
        {
            diffuseSquare(data, x, y);
            x++;
        }
        y++;
    }
    y = 0;
    while (y < SHEIGHT)
    {
        x = 0;
        while (x < SWIDTH)
        {
            data->cSystem[coord(x, y)].density = data->nSystem[coord(x, y)].density;
            x++;
        }
        y++;
    }
}

void advectSquare(t_data *data, int x, int y, int i, int j)
{
    t_uv pos = uv_sum(uv(i, j), uv_mul(data->cSystem[coord(x, y)].u, data->k));
    pos = uv_clamp(1.5, SWIDTH - 1.5, pos);
    t_uv floor = uv((int)pos.x, (int)pos.y);
    t_uv fract = uv(pos.x - (int)pos.x, pos.y - (int)pos.y);
    //print_uv(data->cSystem[coord(x, y)].u);
    double A = data->cSystem[coord(floor.x, abs((int)floor.y + 1 - 100))].density;
    double B = data->cSystem[coord(floor.x + 1,  abs((int)floor.y + 1 - 100))].density;
    double C = data->cSystem[coord(floor.x,  abs((int)floor.y - 100))].density;
    double D = data->cSystem[coord(floor.x + 1,  abs((int)floor.y - 100))].density;
    double z1 = A * (1 - fract.x) + B * fract.x;
    double z2 = C * (1 - fract.x ) + D * fract.x;
    double z = z1 * fract.y + z2 * (1 - fract.y);
    /*if (fract.x || fract.y)
        printf("[%lf][%lf]\n", fract.x, fract.y);*/
    data->nSystem[coord(x, y)].density = z;
}

void curlFree(t_data *data, int x, int y)
{
    t_uv temp;

    
   /*if (y == 98 && x >= 60)
    {
        printf("Curl: [%d][%d]\n", x, y);
        //printf("1\n");
        print_uv(data->cSystem[coord(x + 1,y)].u);
        // printf("2\n");
        print_uv(data->cSystem[coord((x - 1),y)].u);
        // printf("3\n");
        print_uv(data->cSystem[coord(x,y + 1)].u);
        // printf("4\n");
        print_uv(data->cSystem[coord(x,(y - 1))].u);
        printf("\n**************\n");
    }*/
    temp = uv_sum(uv_res(data->cSystem[coord(x + 1,y)].u, data->cSystem[coord((x - 1),y)].u), uv_res(data->cSystem[coord(x,y + 1)].u, data->cSystem[coord(x,(y - 1))].u));
    if (temp.x != 0)
        temp.x /= 2;
    if (temp.y != 0)
        temp.y /= 2;
    data->nSystem[coord(x,y)].u = uv_sum(data->cSystem[coord(x,y)].u, uv_mul(uv_normalized(data->cSystem[coord(x,y)].u), uv_len(temp)));
    /*if (data->nSystem[coord(x,y)].u.x == NAN || data->nSystem[coord(x,y)].u.y == NAN || data->nSystem[coord(x,y)].u.x > 100 || data->nSystem[coord(x,y)].u.x < -100 || data->nSystem[coord(x,y)].u.y < -100)
    {
        print_uv(data->cSystem[coord(x + 1,y)].u);
        print_uv(data->cSystem[coord((x - 1),y)].u);
        print_uv(data->cSystem[coord(x,y + 1)].u);
        print_uv(data->cSystem[coord(x,(y - 1))].u);
        printf("\n**************\n");
        print_uv(temp);
        exit(0);
    }*/
    //print_uv(data->nSystem[coord(x,y)].u);
}

void advection(t_data *data)
{
    int y;
    int x;

    y = 1;
    while (y < SHEIGHT - 1)
    {
        x = 1;
        while (x < SWIDTH - 1)
        {
            advectSquare(data, x, y, x, abs(y - SHEIGHT));
            curlFree(data, x, y);
            //print_uv(data->nSystem[coord(x,y)].u);
            x++;
        }
        y++;
    }
    y = 0;
    while (y < SHEIGHT)
    {
        x = 0;
        while (x < SWIDTH)
        {
            //printf("start[%d][%d]", x, y);
            //print_uv(data->nSystem[coord(x, y)].u);
            data->cSystem[coord(x, y)].density = data->nSystem[coord(x, y)].density;
            data->cSystem[coord(x, y)].u = data->nSystem[coord(x, y)].u;
            data->nSystem[coord(x, y)].density = 0;
            data->nSystem[coord(x, y)].u = uv(0,0);
           // printf("end [%d][%d]", x, y);
           // print_uv(data->cSystem[coord(x, y)].u);
            x++;
        }
        y++;
    }
}