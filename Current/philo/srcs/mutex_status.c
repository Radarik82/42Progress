/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mutex_status.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ariazano <ariazano@student.42wolfsburg.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/15 10:19:06 by ariazano          #+#    #+#             */
/*   Updated: 2024/12/13 10:01:14 by ariazano         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	print_status(char *str, t_philo *philo, int id)
{
	size_t	time;

	pthread_mutex_lock(philo->write_lock);
	time = get_current_time() - philo->time_start;
	if (!death_mutex(philo))
		printf("%zu %d %s\n", time, id, str);
	pthread_mutex_unlock(philo->write_lock);
}

int	philo_death(t_philo *philo, size_t time_to_die)
{
	pthread_mutex_lock(philo->meals_lock);
	if (get_current_time() - philo->meals_last >= time_to_die
		&& philo->eating == 0)
		return (pthread_mutex_unlock(philo->meals_lock), 1);
	pthread_mutex_unlock(philo->meals_lock);
	return (0);
}

int	check_if_dead(t_philo *philo)
{
	int	i;

	i = 0;
	while (i < philo[0].num_philos)
	{
		if (philo_death(&philo[i], philo[i].time_to_die))
		{
			print_status("just died", &philo[i], philo[i].id);
			pthread_mutex_lock(philo[0].dead_lock);
			*philo->dead = 1;
			pthread_mutex_unlock(philo[0].dead_lock);
			return (1);
		}
		i++;
	}
	return (0);
}

int	check_if_all_ate(t_philo *philo)
{
	int	i;
	int	j;

	i = 0;
	j = 0;
	if (philo[0].meals_to_eat == -1)
		return (0);
	while (i < philo[0].num_philos)
	{
		pthread_mutex_lock(philo[i].meals_lock);
		if (philo[i].meals_eaten >= philo[i].meals_to_eat)
			j++;
		pthread_mutex_unlock(philo[i].meals_lock);
		i++;
	}
	if (j == philo[0].num_philos)
	{
		pthread_mutex_lock(philo[0].dead_lock);
		*philo->dead = 1;
		pthread_mutex_unlock(philo[0].dead_lock);
		return (1);
	}
	return (0);
}

void	*check_status(void *flag)
{
	t_philo	*philo;

	philo = (t_philo *)flag;
	while (1)
		if (check_if_dead(philo) == 1 || check_if_all_ate(philo) == 1)
			break ;
	return (flag);
}
