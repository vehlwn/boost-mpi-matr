import matplotlib
import matplotlib.pyplot as plt
import numpy
import scipy.optimize
import collections
import typing
import numpy.polynomial.polynomial as np_poly

model_t = dict[int, dict[int, typing.List[float]]]


def read_file(name: str) -> model_t:
    ret: model_t = collections.defaultdict(lambda: collections.defaultdict(list))
    with open(name, "rt") as f:
        for line in f:
            split = line.split(maxsplit=3)
            world_size = int(split[0])
            width = int(split[1])
            time = float(split[2])
            ret[world_size][width].append(time)
    return ret


avg_model_t = dict[int, typing.List[typing.Tuple[int, float]]]


def make_average(model: model_t) -> avg_model_t:
    ret: avg_model_t = collections.defaultdict(list)
    for (world_size, width_time_dict) in model.items():
        for (width, time_array) in width_time_dict.items():
            ret[world_size].append((width, sum(time_array) / len(time_array)))
        ret[world_size].sort(key=lambda x: x[0])
    return ret


model = read_file("times.txt")
avg_model = make_average(model)


def target_func(x, params):
    return np_poly.polyval(x, params)


def fit(x_data: numpy.ndarray, y_data: numpy.ndarray) -> numpy.ndarray:
    def residual(params):
        return target_func(x_data, params) - y_data

    x0 = numpy.random.normal(size=4)
    result = scipy.optimize.least_squares(residual, x0=x0, method="lm", verbose=2)
    return result.x


def fit_for_world_size(world_size: int, avg_model: avg_model_t) -> numpy.ndarray:
    width_time = avg_model[world_size]
    x_train = numpy.array([pair[0] for pair in width_time])
    y_train = numpy.array([pair[1] for pair in width_time])
    return fit(x_train, y_train)


def plot_time(avg_model: avg_model_t):
    plt.clf()
    plt.title("$T_p(width)$")
    plt.xlabel("Width")
    plt.ylabel("Seconds, $T_p$")
    plt.grid(True, linestyle=":")
    max_colors = len(matplotlib.rcParams["axes.prop_cycle"][:])
    color = 0
    for (world_size, width_time_list) in avg_model.items():
        params = fit_for_world_size(world_size, avg_model)
        x_discrete = [pair[0] for pair in width_time_list]
        y_discrete = [pair[1] for pair in width_time_list]
        x_curve = numpy.linspace(x_discrete[0], x_discrete[-1], 100)
        y_curve = target_func(x_curve, params)
        plt.plot(x_curve, y_curve, label=f"$T_{{{world_size}}}$", color=f"C{color}")
        plt.plot(x_discrete, y_discrete, ".", markersize=5, color=f"C{color}")
        color += 1
        if color == max_colors:
            color = 0
    plt.legend(ncol=3)
    plt.savefig("times.png", dpi=300)


def plot_speedup(avg_model: avg_model_t):
    plt.clf()
    plt.title(r"$S_p = \frac{T_1}{T_p}$")
    plt.xlabel("Processes, p")
    plt.ylabel("$S_p$")
    plt.grid(True, linestyle=":")
    min_world = min(avg_model.keys())
    t_1 = avg_model[min_world][-1][1]
    x_data = list(avg_model.keys())
    y_data = []
    for width_time_list in avg_model.values():
        t_p = width_time_list[-1][1]
        y_data.append(t_1 / t_p)
    plt.plot(x_data, y_data)
    plt.savefig("speedup.png", dpi=300)


plot_time(avg_model)
plot_speedup(avg_model)
