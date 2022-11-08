package com.backup.backupsystemweb.Controller;

import com.backup.backupsystemweb.utils.Algorithm;
import com.backup.backupsystemweb.utils.Judge;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Controller;
import org.springframework.ui.Model;
import org.springframework.web.bind.annotation.*;
import org.springframework.web.servlet.mvc.support.RedirectAttributes;

import java.util.ArrayList;
import java.util.List;

@Controller
public class controller {
    @Autowired
    Judge judge;

    @Autowired
    Algorithm algorithm;
//    private static String password = null;
//    private static String filename = null;

    @GetMapping("/")
    public String main_page() {
        return "index";
    }

    @GetMapping("/test")
    public String test(Model map) {
        List<String> attr = new ArrayList<>();
        attr.add("test");
        map.addAttribute("msg", attr);
        return "test";
    }

    @GetMapping("/thread")
    public String deal(@ModelAttribute("filename") String filename,
                            @ModelAttribute("method") String method,
                            @ModelAttribute("passwd") String password,
                            @ModelAttribute("key") String key,
                            Model map) {
        if(key.equals("recover") && method.equals("jiemi")) {
            boolean right = judge.CheckPasswd(filename, password);
            if(!right) {
                List<String> attr = new ArrayList<String>();
                attr.add("密码有误，请检查后重试");
                map.addAttribute("msg", attr);
                return "test";
            }
        }
        List<String> attr = new ArrayList<String>();
        attr.add(key);
        attr.add(filename);
        attr.add(method);
        attr.add(password);
        List<String> resp = algorithm.deal(attr);
        if(key.equals("backup") && resp.get(0).equals("执行成功")) {
            judge.InDatabase(filename, method, password);
        }
        map.addAttribute("msg", resp);
        return "test";
    }

    @PostMapping("/beifen")
    public String beifen(@RequestParam("FileDir") String filePath,
                                @RequestParam("method") String method,
                                Model map,
                                RedirectAttributes attr) {
//        filename = filePath;
        if(filePath.equals("")) {
            List<String> tmp = new ArrayList<>();
            tmp.add("文件路径不能为空，请检查后重试！");
            map.addAttribute("msg", tmp);
            return "test";
        }
        filePath = judge.normal(filePath);
        boolean passwd =  judge.UsePasswd(filePath, method);
        if(passwd) {
            map.addAttribute("filename", filePath);
            map.addAttribute("method", method);
            map.addAttribute("key", "backup");
            return "passwd";
        }
        else {
            attr.addFlashAttribute("filename", filePath);
            attr.addFlashAttribute("method", method);
            attr.addFlashAttribute("passwd", null);
            attr.addFlashAttribute("key", "backup");
            return "redirect:/thread";
        }
    }

    @PostMapping("/huifu")
    public String huifu(@RequestParam("FileDir2") String filePath,
                                @RequestParam("leixing") String method,
                                Model map,
                                RedirectAttributes attr) {
        if(filePath.equals("")) {
            List<String> tmp = new ArrayList<>();
            tmp.add("文件路径不能为空，请检查后重试！");
            map.addAttribute("msg", tmp);
            return "test";
        }
        filePath = judge.normal(filePath);
        // 用于判断用户选择的恢复方式是否是输入密码的方式
        boolean passwd =  judge.UsePasswd(filePath, method);
        // 用于判断用户操作的文件是否加密备份
        boolean usePass = judge.HavePasswd(filePath);
        if(!judge.GetLevel(filePath).equals(method)) {
            List<String> att = new ArrayList<>();
            att.add("很抱歉，您之前对"+filePath+"进行的备份方式与您选择的恢复方式不符，请选择其他两种恢复模式后，重新进行恢复");
            map.addAttribute("msg", att);
            return "test";
        }
        if(usePass) {
            // 是加密备份且用户选择输入密码
            if(passwd) {
                map.addAttribute("filename", filePath);
                map.addAttribute("method", method);
                map.addAttribute("key", "recover");
                return "passwd";
            } else {    // 是加密备份但用户选择不输入密码
                List<String> att = new ArrayList<>();
                att.add("很抱歉，您之前对"+filePath+"进行了加密备份，请选择‘解密解压’恢复模式后，重新进行恢复");
                map.addAttribute("msg", att);
                return "test";
            }
        } else  // 未进行加密备份时，直接忽略用户输入的密码
        {
            if(passwd) {
                List<String> att = new ArrayList<>();
                att.add("很抱歉，您之前对"+filePath+"进行了非加密备份，请选择其他两种恢复模式后，重新进行恢复");
                map.addAttribute("msg", att);
                return "test";
            }
            attr.addFlashAttribute("filename", filePath);
            attr.addFlashAttribute("method", method);
            attr.addFlashAttribute("passwd", null);
            attr.addFlashAttribute("key", "recover");
            return "redirect:/thread";
        }
    }

    @PostMapping("/check")
    public String check(@RequestParam("tarFile") String filePath,
                                Model map,
                                RedirectAttributes attr) {
        if(filePath.equals("")) {
            List<String> tmp = new ArrayList<>();
            tmp.add("文件路径不能为空，请检查后重试！");
            map.addAttribute("msg", tmp);
            return "test";
        }
        filePath = judge.normal(filePath);
        String passwd = null, method = "jiemi";
        if(judge.HavePasswd(filePath)) {
            passwd = judge.GetPasswd(filePath);
        } else {
            method = judge.GetLevel(filePath);
        }
        attr.addFlashAttribute("filename", filePath);
        attr.addFlashAttribute("method", method);
        attr.addFlashAttribute("passwd", passwd);
        attr.addFlashAttribute("key", "check");
        return "redirect:/thread";
    }

    @PostMapping("/pass")
    public String getpasswd(@RequestParam("passwd") String passwd,
                                 @RequestParam("filename") String filename,
                                 @RequestParam("method") String method,
                                 @RequestParam("key") String key,
                                 RedirectAttributes attr,
                                Model map) {
        if(passwd.equals("")) {
            List<String> tmp = new ArrayList<>();
            tmp.add("密码不能为空，请检查后重试！");
            map.addAttribute("msg", tmp);
            return "test";
        }
        attr.addFlashAttribute("filename", filename);
        attr.addFlashAttribute("method", method);
        attr.addFlashAttribute("passwd", passwd);
        attr.addFlashAttribute("key", key);
        return "redirect:/thread";
    }
}
