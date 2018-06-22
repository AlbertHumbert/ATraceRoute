package util;

public class LibResolver {

    public enum OSType {
        WIN("Windows", "dll"),
        LINUX("Linux", "so"),
        OSX("OSX", "dylib"),
        UNKNOWN("Unknown", "?");

        String desc;
        String libType;

        OSType(String desc, String libType) {
            this.desc = desc;
            this.libType = libType;
        }

        /**
         * @return 系统名称
         */
        public String getDesc() {
            return desc;
        }

        /**
         * @return 动态链接库文件后缀
         */
        public String getLibType() {
            return libType;
        }
    }

    public static OSType getOsType() {

        String os = System.getProperty("os.name").toLowerCase();

        if (os.contains("mac") && os.contains("x")) {
            return OSType.OSX;
        }

        if (os.contains("windows")) {
            return OSType.WIN;
        }

        if (os.contains("linux")) {
            return OSType.LINUX;
        }

        return OSType.UNKNOWN;
    }
}
