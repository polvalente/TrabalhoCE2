net = 'seno.res';
fid = fopen(net, 'r');
[t, 0, 1, 2, jV0100] = textscan(fid,'%s %s %s %s %s');
T = cellfun(@str2num, {t{2:end}});
E1 = cellfun(@str2num, {e1{2:end}});
plot(T,E1);