datFiles = {
	% 'vmaf-time-limit-05000.dat',
	'vmaf-time-limit-06000.dat',
	% 'vmaf-time-limit-07000.dat',
	'vmaf-time-limit-08000.dat',
	% 'vmaf-time-limit-09000.dat',
	'vmaf-time-limit-10000.dat',
	% 'vmaf-time-limit-11000.dat',
	'vmaf-time-limit-12000.dat'
	% 'vmaf-time-limit-13000.dat',
	'vmaf-time-limit-14000.dat',
	% 'vmaf-time-limit-15000.dat'
};

hf = figure();

title("Encoding time in function of VMAF limit");
xlabel("VMAF limit (precent)");
ylabel("Encoding time (ms)");

% Get VMAF index
l = load(datFiles{1,1});
vmaf_limit = l(:,1);

hold on;
for i = 1:length(datFiles)
	l = load(datFiles{i,1});
	bitstream_size = l(:,2);

	plot(vmaf_limit, bitstream_size);
end
hold off;
legend("encoding x0.6", "encoding x0.8", "encoding x1.0", "encoding x1.2", "encoding x1.4", "location", "northwest");

print(hf, "pareto-encoding-time.pdf", "-dpdflatexstandalone");
system("pdflatex pareto-encoding-time");
