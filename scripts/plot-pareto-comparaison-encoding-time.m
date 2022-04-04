clf;

exps = {
	"pareto-cpu-i5-4460";
	"pareto-cpu-i7-6700";
	"pareto-cpu-i7-12700K";
	"pareto-cpu-i9-9900";
};

for j = 4000:2000:16000
	clf;
	hold on;
	for i = 1:length(exps)
		exp = exps{i};
		filename = sprintf("%s/fixed-encoding-time-%05d.dat", exp, j);
		res = importdata(filename, "\t");
		plot(res.data(:,2)/10^6, res.data(:,1), "-o");
	endfor

	% plot([0 ; 3], [10 ; 10], "k--");
	plot([1.3 ; 1.3], [0 ; 100], "k--");

	hold off;

	xlim([0 5])
	ylim([70 100])

	t = sprintf("Pareto front comparaison with fixed encoding time at %0.1f s", j/10^3);
	title(t)
	xlabel("Bitstream size (Mo)")
	ylabel("VMAF score")
	legend("i5-4460","i7-6700","i7-12700K","i9-9900", "location", "southeast")

	filename = sprintf("pareto-comparaison-encoding-time-%05d.png", j);
	saveas(1, filename);
endfor
