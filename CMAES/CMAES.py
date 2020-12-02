import numpy as np

# you must use python 3.6, 3.7, 3.8(3.8 not for macOS) for sourcedefender
import sourcedefender
from HomeworkFramework import Function



class RS_optimizer(Function): # need to inherit this class "Function"
    def __init__(self, target_func):
        super().__init__(target_func) # must have this init to work normally

        self.lower = self.f.lower(target_func) # input 的上限，而不是function的上限
        self.upper = self.f.upper(target_func)
        self.dim = self.f.dimension(target_func)

        self.target_func = target_func

        self.eval_times = 0
        self.optimal_value = float("inf")
        self.optimal_solution = np.empty(self.dim)

        

    def get_optimal(self):
        return self.optimal_solution, self.optimal_value

    # # Random search method
    # def run(self, FES): # main part for your implementation
        
    #     while self.eval_times < FES:
    #         print('=====================FE=====================')
    #         print(self.eval_times)

    #         # sol means input 
    #         solution = np.random.uniform(np.full(self.dim, self.lower), np.full(self.dim, self.upper), self.dim)
    #         # print(type(solution))
    #         value = self.f.evaluate(func_num, solution)
    #         # value2 = self.f.evaluate(func_num, solution)
    #         # print(value2)
    #         self.eval_times += 1

    #         if value == "ReachFunctionLimit":
    #             print("ReachFunctionLimit")
    #             break            
    #         if float(value) < self.optimal_value:
    #             self.optimal_solution[:] = solution
    #             self.optimal_value = float(value)

    #         print("optimal: %f\n" % self.get_optimal()[1])


    # CMAES
    def run(self, FES): # main part for your implementation
        # user set input
        # m = np.random.uniform(np.full(self.dim, self.lower), np.full(self.dim, self.upper), self.dim).reshape(self.dim ,1)
        m = np.full((self.dim ,1) ,(self.upper + self.lower)/2)
        # m = np.random.normal((self.lower + self.upper) ,(self.upper - self.lower) ,(self.dim ,1))
        # sigma = (self.upper - self.lower) / 2 # how to set
        # sigma = 0.1
        sigma = np.sqrt((self.upper - self.lower) / 2)
        _lambda = 100
        
        # check every element not exceed upper/lower bound
        for k in range(self.dim):
            if m[k][0] > self.upper:
                m[k][0] = self.upper
            if m[k][0] < self.lower:
                m[k][0] = self.lower
        
        
        # init
        C = np.identity(self.dim)
        p_c = np.zeros((self.dim ,1))
        p_sigma = np.zeros((self.dim ,1))

        # Set
        n = self.dim
        Cc = 4 / n
        c_sigma = 4 / n
        c1 = 2 / np.power(n ,2)
        mu = int(_lambda / 4) # 取一半的sample數
        w = np.array([mu+1/2 - np.log(i+1) for i in range(mu)])
        w = w / sum(w)
        mu_w = sum(np.power(w ,2))
        c_mu = mu_w / np.power(n ,2)
        d_sig = 1 + np.power( mu_w / n ,1/2)

        while self.eval_times < FES:
            # sampling
            sample = list()
            for i in range(_lambda):
                y = np.random.multivariate_normal(np.zeros(self.dim) ,C).reshape(self.dim ,1)
                x = m + sigma * y

                # check element exceed upper/lower bound
                for k in range(self.dim):
                    if x[k][0] > self.upper:
                        x[k][0] = self.upper
                    if x[k][0] < self.lower:
                        x[k][0] = self.lower
                
                self.eval_times += 1
                if self.eval_times > FES:
                    break

                value = float(self.f.evaluate(func_num ,x.reshape(self.dim)))
                sample.append((x ,y ,value))
            sample = sorted(sample ,key = lambda s:s[2] ,reverse=False)
            

            # update mean
            y_w = np.zeros((n,1))
            for i in range(mu):
                y_w += w[i] * sample[i][1]
            m = m + sigma * y_w


            # cumulation for C
            if np.linalg.norm(p_sigma) < 1.5 * np.power(n ,1/2):
                temp = np.power(1 - Cc ,2)
                p_c = (1 - Cc) * p_c + np.power(1 - temp ,1/2) * np.power(mu_w ,1/2) * y_w
            else:
                p_c = (1 - Cc) * p_c


            # cumlation for sigma
            eig_value ,eig_vector = np.linalg.eig(C)
            eig_value = np.diag(eig_value)
            temp = np.power(1 - c_sigma ,2)
            p_sigma = (1 - c_sigma) * p_sigma + np.power(1 - temp ,1/2) * np.power(mu_w, 1/2) * eig_vector.dot(np.power(eig_value ,-1/2)).dot(np.linalg.inv(eig_vector)).dot(y_w)


            # update C
            temp = np.zeros(C.shape)
            for i in range(mu):
                temp += w[i] * sample[i][1].dot(np.transpose(sample[i][1]))
            C = (1 - c1 - c_mu) * C + c1 * p_c.dot(np.transpose(p_c)) + c_mu * temp


            # update sigma
            sigma = sigma * np.exp((c_sigma / d_sig) * ((np.linalg.norm(p_c)/np.linalg.norm(np.random.multivariate_normal(np.zeros(self.dim) ,np.identity(self.dim)))) -1 )) # normal可能有問題

            if float(sample[0][2]) < self.optimal_value:
                print("optimal_solution change")
                self.optimal_solution[:] = sample[0][0].reshape(self.dim)
                self.optimal_value = sample[0][2]

            # print("optimal: %f\n" % self.get_optimal()[1])

            
            

if __name__ == '__main__':
    func_num = 1
    fes = 0
    #function1: 1000, function2: 1500, function3: 2000, function4: 2500
    while func_num < 5:
        if func_num == 1:
            fes = 1000
        elif func_num == 2:
            fes = 1500
        elif func_num == 3:
            fes = 2000 
        else:
            fes = 2500

        # you should implement your optimizer
        op = RS_optimizer(func_num)
        op.run(fes)
        
        best_input, best_value = op.get_optimal()
        print("best_input: " ,best_input) # input parameter
        print("best_value: " , best_value) # input value
        print("")

        
        # change the name of this file to your student_ID and it will output properlly
        with open("{}_function{}.txt".format(__file__.split('_')[0], func_num), 'w+') as f:
            for i in range(op.dim):
                f.write("{}\n".format(best_input[i]))
            f.write("{}\n".format(best_value))
        func_num += 1 
